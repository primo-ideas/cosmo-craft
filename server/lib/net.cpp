#include "net.hpp"

#include <cstdlib>
#include <memory>
#include <mutex>

#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

#include "log.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace asio = boost::asio;
using tcp = boost::asio::ip::tcp;

namespace cosmo {

Session::Session(tcp::socket &&socket)
    : ws_(std::move(socket))
    , authenticated_(false) {
}

void Session::run() {
    asio::dispatch(ws_.get_executor(),
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
    logger.trace("HANDSHAKE");
    if (ec) {
        logger.warn("Handhshake error: {}", ec.message());
        return;
    }
    do_read();
}

void Session::do_read() {
    ws_.async_read(buffer_, beast::bind_front_handler(&Session::on_read, shared_from_this()));
}

void Session::on_read(beast::error_code ec, std::size_t bytes_read) {
    logger.trace("READ ({} bytes)", bytes_read);
    if (ec == websocket::error::closed) {
        logger.debug("Connection closed");
        return;
    }

    if (ec) {
        logger.trace("Read error: {}", ec.message());
        return;
    }

    if (!ws_.got_text()) {
        close(boost::beast::websocket::close_code::unknown_data);
    }
    auto buffer_str =
        std::string(static_cast<char const *>(buffer_.cdata().data()), buffer_.cdata().size());
    buffer_.consume(bytes_read);
}

void Session::close(boost::beast::websocket::close_code code) {
    ws_.async_close(boost::beast::websocket::close_reason(code),
                    beast::bind_front_handler(&Session::on_close, shared_from_this()));
}
void Session::on_close(beast::error_code ec) {
    logger.trace("Closed");
}

void Session::on_write(beast::error_code ec, std::size_t bytes_transferred) {
    logger.trace("WRITE ({} bytes)", bytes_transferred);

    if (ec) {
        logger.warn("Write error: {}", ec.message());
        return;
    }

    buffer_.consume(buffer_.size());

    do_read();
}

std::vector<std::string> Session::pop_messages() {
    std::lock_guard lock(read_mutex_);
    std::vector<std::string> messages;
    while (!to_read_.empty()) {
        auto front = to_read_.front();
        messages.emplace_back(front);
        to_read_.pop();
    }
    return messages;
}

void Session::push_message(std::string const &msg) {
    std::lock_guard lock(write_mutex_);

    to_write_.push(msg);
}

bool Session::authenticated() const {
    return authenticated_;
}

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
    return sessions_;
}

} // namespace cosmo
