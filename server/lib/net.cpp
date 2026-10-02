
#include "net.hpp"

#include <cstdlib>
#include <memory>
#include <string>
#include <utility>

#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <spdlog/spdlog.h>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

namespace cosmo {
namespace transport {

Session::Session(tcp::socket &&socket)
    : ws_(std::move(socket)) {
}

void Session::run() {
    net::dispatch(ws_.get_executor(),
                  beast::bind_front_handler(&Session::on_run, shared_from_this()));
}

void Session::on_run() {
    ws_.set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));

    ws_.set_option(websocket::stream_base::decorator([](websocket::response_type &res) {
        res.set(http::field::server,
                std::string(BOOST_BEAST_VERSION_STRING) + " websocket-server-async");
    }));

    ws_.async_accept(beast::bind_front_handler(&Session::on_accept, shared_from_this()));
}

void Session::on_accept(beast::error_code ec) {
    spdlog::trace("HANDSHAKE");
    if (ec) {
        spdlog::warn("Handhshake error: {}", ec.message());
        return;
    }
    do_read();
}

void Session::do_read() {
    ws_.async_read(buffer_, beast::bind_front_handler(&Session::on_read, shared_from_this()));
}

void Session::on_read(beast::error_code ec, std::size_t bytes_read) {
    spdlog::trace("READ ({} bytes)", bytes_read);
    if (ec == websocket::error::closed) {
        spdlog::debug("Connection closed");
        return;
    }

    if (ec) {
        spdlog::warn("Read error: {}", ec.message());
        return;
    }

    ws_.text(ws_.got_text());
    ws_.async_write(buffer_.data(),
                    beast::bind_front_handler(&Session::on_write, shared_from_this()));
}

void Session::on_write(beast::error_code ec, std::size_t bytes_transferred) {
    spdlog::trace("WRITE ({} bytes)", bytes_transferred);

    if (ec) {
        spdlog::warn("Write error: {}", ec.message());
        return;
    }

    buffer_.consume(buffer_.size());

    do_read();
}

Listener::Listener(net::io_context &ioc, tcp::acceptor acceptor)
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
    acceptor_.async_accept(net::make_strand(ioc_),
                           beast::bind_front_handler(&Listener::on_accept, shared_from_this()));
}

void Listener::on_accept(beast::error_code ec, tcp::socket socket) {
    if (ec) {
        spdlog::warn("Accept error {}", ec.message());
        return;
    } else {
        spdlog::debug("New connection");
        std::make_shared<Session>(std::move(socket))->run();
    }

    do_accept();
}

} // namespace transport
} // namespace cosmo
