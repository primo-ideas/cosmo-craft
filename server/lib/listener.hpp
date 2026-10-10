#pragma once

#include <memory>

#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

#include "game.hpp"
#include "session.hpp"

namespace cosmo {

class Listener : public std::enable_shared_from_this<Listener> {
    boost::asio::io_context       &ioc_;
    boost::asio::ip::tcp::acceptor acceptor_;
    std::shared_ptr<Game>          game_;

  public:
    Listener(boost::asio::io_context &ioc, boost::asio::ip::tcp::acceptor &&acceptor,
             std::shared_ptr<Game> game);
    void                                  run();
    boost::asio::ip::tcp::acceptor const &acceptor();
    boost::asio::io_context              &ioc();

    std::vector<std::shared_ptr<Session>> sessions();

  private:
    void do_accept();
    void on_accept(boost::beast::error_code ec, boost::asio::ip::tcp::socket socket);
};

} // namespace cosmo
