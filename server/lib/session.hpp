
#pragma once

#include <cstdlib>
#include <memory>
#include <queue>

#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

namespace cosmo {

class Session : public std::enable_shared_from_this<Session> {
    boost::beast::websocket::stream<boost::beast::tcp_stream> ws_;
    boost::beast::flat_buffer buffer_;
    std::queue<std::string> to_read_;
    std::mutex read_mutex_;
    std::queue<std::string> to_write_;
    bool writing_ = false;
    bool closing_ = false;
    bool authenticated_ = false;

  public:
    explicit Session(boost::asio::ip::tcp::socket &&socket);
    void run();
    void close();
    void on_close(boost::beast::error_code ec);
    void on_run();
    void on_accept(boost::beast::error_code ec);
    void do_read();
    void do_write();
    void on_read(boost::beast::error_code ec, std::size_t bytes_transferred);
    void on_write(boost::beast::error_code ec, std::size_t bytes_transferred);
    std::vector<std::string> pop_messages();
    void push_message(std::string const &msg);
    bool authenticated() const;
};

} // namespace cosmo
