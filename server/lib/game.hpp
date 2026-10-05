#pragma once

#include <memory>
#include <set>

#include <boost/asio/io_context.hpp>
#include <nlohmann/json.hpp>

#include "listener.hpp"

namespace cosmo {

class Game {
  private:
    std::shared_ptr<Listener> listener_;
    std::set<std::string> nicknames_;

  public:
    Game(std::shared_ptr<Listener> listener);
    void cycle();
};

} // namespace cosmo
