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

template <class T> std::string serialize(T const &value) {
    json json = value;
    return json.dump();
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
                auto response = AuthResponse();
                response.result = true;
                auto maybe_auth = deserialize<ClientAuth>(msg);
                if (!maybe_auth.has_value()) {
                    logger.trace("Serde error: {}", (int)maybe_auth.error());
                    response.result = false;
                    session->close(boost::beast::websocket::close_code::unknown_data);
                }
                auto auth = maybe_auth.value();
                if (nicknames_.find(auth.nickname) != nicknames_.end()) {
                    response.result = false;
                    session->close(boost::beast::websocket::close_code::unknown_data);
                }
                auto response_str = serialize(response);
                session->push_message(response_str);
            }
        }
    }
}

} // namespace cosmo
