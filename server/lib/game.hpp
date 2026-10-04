#pragma once

#include <expected>
#include <memory>
#include <set>

#include <boost/asio/io_context.hpp>
#include <nlohmann/json.hpp>

#include "listener.hpp"

namespace cosmo {

struct ClientAuth {
    std::string nickname;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ClientAuth, nickname)

struct AuthResponse {
    bool result;
    std::string message;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AuthResponse, result, message)

enum class SerdeError {
    InvalidJson,
    InvalidInput,
};

template <class T> std::expected<T, SerdeError> deserialize(std::string const &json_str) {
    auto parse_result = nlohmann::json::parse(json_str, nullptr, false);
    if (parse_result.is_discarded()) {
        return std::unexpected(SerdeError::InvalidJson);
    }
    try {
        auto value = parse_result.get<T>();
        return value;
    } catch (...) {
        return std::unexpected(SerdeError::InvalidInput);
    }
}

template <class T> std::string serialize(T const &value) {
    nlohmann::json json = value;
    return json.dump();
}

class Game {
  private:
    std::shared_ptr<Listener> listener_;
    std::set<std::string> nicknames_;

  public:
    Game(std::shared_ptr<Listener> listener);
    void cycle();
};

} // namespace cosmo
