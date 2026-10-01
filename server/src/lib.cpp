#include "lib.hpp"

#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

#include "net.hpp"

namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

namespace cosmo {

struct Instance::Impl {
    net::io_context ioc;
    std::shared_ptr<transport::Listener> listener;
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

Instance Instance::launch() {
    return launch("127.0.0.1", 0);
}

Instance Instance::launch(std::string const &bind_addr, unsigned short port) {
    auto nb_thr = std::thread::hardware_concurrency();
    if (nb_thr == 0) {
        nb_thr = 1;
    }

    auto impl = std::make_unique<Impl>(nb_thr);

    auto listener = std::make_shared<transport::Listener>(
        impl->ioc, tcp::endpoint{net::ip::make_address(bind_addr), port});
    listener->run();

    std::vector<std::thread> v;
    v.reserve(nb_thr);
    for (unsigned i = 0; i < nb_thr; ++i)
        v.emplace_back([&ioc = impl->ioc] { ioc.run(); });

    impl->listener = listener;
    impl->threads = std::move(v);
    impl->port = impl->listener->acceptor().local_endpoint().port();
    return Instance(std::move(impl));
}

} // namespace cosmo
