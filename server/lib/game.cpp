#include "game.hpp"

#include <expected>

#include "log.hpp"

using nlohmann::json;

namespace cosmo {

enum class SerdeError {
    InvalidJson,
    InvalidInput,
};

template <class T> std::expected<T, SerdeError> deserialize(std::string const &json_str) {
    auto parse_result = json::parse(json_str, nullptr, false);
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

Game::Game(std::shared_ptr<Listener> listener)
    : listener_(listener) {
}

void Game::cycle() {
    auto sessions = listener_->sessions();

    for (auto &session : sessions) {
        auto messages = session->pop_messages();
        for (auto msg : messages) {
            if (!session->authenticated()) {
                auto maybe_auth = deserialize<ClientAuth>(msg);
                if (!maybe_auth.has_value()) {
                    logger.trace("Serde error: {}", (int)maybe_auth.error());
                    session->close(boost::beast::websocket::close_code::unknown_data);
                }
            }
        }
    }
}

} // namespace cosmo
