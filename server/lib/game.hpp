#pragma once

#include <memory>

#include <boost/asio/io_context.hpp>
#include <nlohmann/json.hpp>

#include "net.hpp"
#include "nlohmann/json.hpp"

namespace cosmo {

struct ClientAuth {
    std::string nickname;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ClientAuth, nickname)

class Game {
  private:
    std::shared_ptr<Listener> listener_;
    std::vector<std::string> nicknames_;

  public:
    Game(std::shared_ptr<Listener> listener);
    void cycle();
};

} // namespace cosmo
