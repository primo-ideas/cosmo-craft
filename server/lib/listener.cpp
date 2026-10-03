#include "listener.hpp"

#include <memory>
#include <mutex>

#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

#include "log.hpp"

namespace beast = boost::beast;
namespace asio = boost::asio;
using tcp = boost::asio::ip::tcp;

namespace cosmo {

Listener::Listener(asio::io_context &ioc, tcp::acceptor acceptor)
    : ioc_(ioc)
    , acceptor_(std::move(acceptor)) {
}

void Listener::run() {
    do_accept();
}

tcp::acceptor const &Listener::acceptor() {
    return acceptor_;
}

boost::asio::io_context &Listener::ioc() {
    return ioc_;
}

void Listener::do_accept() {
    acceptor_.async_accept(asio::make_strand(ioc_),
                           beast::bind_front_handler(&Listener::on_accept, shared_from_this()));
}

void Listener::on_accept(beast::error_code ec, tcp::socket socket) {
    if (ec) {
        logger.warn("Accept error {}", ec.message());
        return;
    } else {
        logger.debug("New connection");
        std::make_shared<Session>(std::move(socket))->run();
    }

    do_accept();
}

std::vector<std::shared_ptr<Session>> Listener::sessions() {
    std::lock_guard lock(session_mutex_);
    auto sessions = sessions_;
    return sessions;
}

} // namespace cosmo
