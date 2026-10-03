#include "session.hpp"

#include "log.hpp"


namespace beast = boost::beast;
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
    ws_.set_option(beast::websocket::stream_base::timeout::suggested(beast::role_type::server));

    ws_.set_option(
        beast::websocket::stream_base::decorator([](beast::websocket::response_type &res) {
            res.set(beast::http::field::server,
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
    if (ec == beast::websocket::error::closed) {
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

} // namespace cosmo
