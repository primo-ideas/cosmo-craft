#pragma once

#include <boost/asio/io_context.hpp>

#include "net.hpp"


namespace cosmo {

extern void run_game(std::shared_ptr<Listener> listener);

}
