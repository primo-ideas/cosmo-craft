#include "listener.hpp"

#include <memory>

#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

#include "log.hpp"

namespace beast = boost::beast;
namespace asio  = boost::asio;
using tcp       = boost::asio::ip::tcp;

namespace cosmo {

Listener::Listener(boost::asio::io_context &ioc, boost::asio::ip::tcp::acceptor &&acceptor,
                   std::shared_ptr<Game> game)
    : ioc_(ioc)
    , acceptor_(std::move(acceptor))
    , game_(game) {
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
        auto session = std::make_shared<Session>(std::move(socket), game_);
        game_->new_session(session);
    }

    do_accept();
}

} // namespace cosmo
