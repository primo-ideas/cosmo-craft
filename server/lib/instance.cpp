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

#include "core.hpp"
#include "game.hpp"
#include "log.hpp"
#include "net.hpp"

namespace beast = boost::beast;
namespace asio = boost::asio;
using tcp = boost::asio::ip::tcp;

namespace cosmo {

struct Instance::Impl {
    asio::io_context ioc;
    std::shared_ptr<Listener> listener;
    std::vector<std::thread> threads;
    unsigned short port;

    explicit Impl(int concurrency)
        : ioc(concurrency) {
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

    auto listener = std::make_shared<Listener>(impl->ioc, std::move(acceptor));
    listener->run();
    logger.info("Listenning on {}:{}", bind_addr, port);

    for (unsigned i = 0; i < nb_thr - 1; ++i)
        impl->threads.emplace_back([&ioc = impl->ioc] { ioc.run(); });
    impl->threads.emplace_back([&ioc = impl->ioc] { run_game(ioc); });

    impl->listener = listener;
    impl->port = impl->listener->acceptor().local_endpoint().port();
    return Instance(std::move(impl));
}

} // namespace cosmo
