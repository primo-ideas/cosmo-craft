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
    std::unique_ptr<net::io_context> ioc;
    std::shared_ptr<transport::Listener> listener;
    std::vector<std::thread> threads;
};

Instance::Instance(Impl &&impl)
    : impl_(std::make_unique<Impl>(std::move(impl))) {
}

Instance::~Instance() {
    // stop();
}

unsigned short Instance::port() const {
    return impl_->listener->acceptor().local_endpoint().port();
}

void Instance::stop() {
    impl_->listener->ioc().stop();
    for (auto &t : impl_->threads)
        t.join();
}

[[nodiscard]]
Instance Instance::launch() {
    return launch("127.0.0.1", 0);
}

[[nodiscard]]
Instance Instance::launch(std::string const &bind_addr, unsigned short port) {
    auto nb_thr = std::thread::hardware_concurrency();
    if (nb_thr == 0) {
        nb_thr = 1;
    }
    auto ioc = std::make_unique<net::io_context>(static_cast<int>(nb_thr));

    auto listener = std::make_shared<transport::Listener>(
        *ioc.get(), tcp::endpoint{net::ip::make_address(bind_addr), port});
    listener->run();

    std::vector<std::thread> v;
    v.reserve(nb_thr);
    for (auto i = nb_thr - 1; i > 0; --i)
        v.emplace_back([&ioc = *ioc.get()] { ioc.run(); });

    Instance::Impl impl;
    impl.ioc = std::move(ioc);
    impl.listener = listener;
    impl.threads = std::move(v);
    return Instance(std::move(impl));
}

} // namespace cosmo
