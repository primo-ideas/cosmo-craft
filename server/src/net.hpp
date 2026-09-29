#pragma once

#include <cstdlib>
#include <memory>

#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

namespace cosmo {
namespace transport {

class Session : public std::enable_shared_from_this<Session> {
    boost::beast::websocket::stream<boost::beast::tcp_stream> ws_;
    boost::beast::flat_buffer buffer_;

  public:
    explicit Session(boost::asio::ip::tcp::socket &&socket);
    void run();
    void on_run();
    void on_accept(boost::beast::error_code ec);
    void do_read();
    void on_read(boost::beast::error_code ec, std::size_t bytes_transferred);
    void on_write(boost::beast::error_code ec, std::size_t bytes_transferred);
};

class Listener : public std::enable_shared_from_this<Listener> {
    boost::asio::io_context &ioc_;
    boost::asio::ip::tcp::acceptor acceptor_;

  public:
    Listener(boost::asio::io_context &ioc, boost::asio::ip::tcp::endpoint endpoint);
    void run();
    boost::asio::ip::tcp::acceptor const &acceptor();
    boost::asio::io_context &ioc();

  private:
    void do_accept();
    void on_accept(boost::beast::error_code ec, boost::asio::ip::tcp::socket socket);
};

} // namespace transport

} // namespace cosmo
