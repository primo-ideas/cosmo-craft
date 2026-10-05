#include <chrono>
#include <expected>
#include <memory>
#include <string>
#include <system_error>
#include <thread>
#include <utility>
#include <vector>

#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <spdlog/spdlog.h>

#include "cosmo.hpp"
#include "game.hpp"
#include "listener.hpp"
#include "log.hpp"

namespace beast = boost::beast;
namespace asio = boost::asio;
using tcp = boost::asio::ip::tcp;

namespace cosmo {

struct Instance::Impl {
    asio::io_context ioc;
    std::shared_ptr<Listener> listener;
    std::vector<std::thread> threads;
    unsigned short port;
    asio::steady_timer cycle_timer;
    std::function<void()> cycle_handler;

    explicit Impl(int concurrency)
        : ioc(concurrency)
        , cycle_timer(ioc) {
    }
    ~Impl() {
        ioc.stop();
        join();
    }
    void join() {
        for (auto &t : threads)
            if (t.joinable())
                t.join();
    }

    void set_cycle_handler(std::function<void()> func) {
        cycle_handler = func;
    }

    void set_cycle_timer() {
        cycle_timer.expires_after(std::chrono::milliseconds(100));
        cycle_timer.async_wait(std::bind(&Instance::Impl::on_timer, this, std::placeholders::_1));
    }

    void on_timer(boost::system::error_code ec) {
        if (ec)
            return;
        cycle_handler();
        set_cycle_timer();
    }
};

Instance::Instance(std::unique_ptr<Impl> impl)
    : impl_(std::move(impl)) {
}

Instance::Instance(Instance &&) noexcept = default;
Instance &Instance::operator=(Instance &&) noexcept = default;
Instance::~Instance() = default;

unsigned short Instance::port() const {
    return impl_->port;
}

void Instance::stop() {
    impl_->listener->ioc().stop();
}
void Instance::wait() {
    for (auto &t : impl_->threads)
        t.join();
}

std::expected<Instance, std::error_code> Instance::launch(unsigned short port,
                                                          std::string const &bind_addr) {
    auto nb_thr = std::thread::hardware_concurrency();
    if (nb_thr == 0) {
        nb_thr = 1;
    }

    auto impl = std::make_unique<Impl>(nb_thr);

    beast::error_code ec;

    beast::net::ip::tcp::acceptor acceptor(impl->ioc);
    tcp::endpoint endpoint{asio::ip::make_address(bind_addr), port};
    if (acceptor.open(endpoint.protocol(), ec))
        return std::unexpected(ec);
    if (acceptor.set_option(asio::socket_base::reuse_address(true), ec))
        return std::unexpected(ec);
    if (acceptor.bind(endpoint, ec))
        return std::unexpected(ec);
    if (acceptor.listen(asio::socket_base::max_listen_connections, ec))
        return std::unexpected(ec);

    logger.info("Listening on {}:{}", bind_addr, acceptor.local_endpoint().port());

    auto listener = std::make_shared<Listener>(impl->ioc, std::move(acceptor));
    listener->run();

    auto game = std::make_shared<Game>(listener);
    impl->set_cycle_handler(std::bind(&Game::cycle, game));
    impl->set_cycle_timer();

    for (unsigned i = 0; i < nb_thr - 1; ++i)
        impl->threads.emplace_back([&ioc = impl->ioc] { ioc.run(); });

    impl->listener = listener;
    impl->port = impl->listener->acceptor().local_endpoint().port();
    return Instance(std::move(impl));
}

} // namespace cosmo
