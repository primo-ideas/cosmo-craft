#pragma once

#include <boost/asio/io_context.hpp>
#include <nlohmann/json.hpp>

#include "net.hpp"
#include "nlohmann/json.hpp"

namespace cosmo {

struct ClientAuth {
    std::string nickname;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ClientAuth, nickname)

extern void run_game(std::shared_ptr<Listener> listener);

} // namespace cosmo
