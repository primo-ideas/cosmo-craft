#include "game.hpp"

#include <expected>

#include "log.hpp"
#include "session.hpp"

using nlohmann::json;

namespace cosmo {

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
                response.message = "Welcome";
                auto maybe_auth = deserialize<ClientAuth>(msg);
                if (!maybe_auth.has_value()) {
                    logger.trace("Serde error: {}", (int)maybe_auth.error());
                    response.result = false;
                    response.message = "Invalid data";
                }
                auto auth = maybe_auth.value();
                if (nicknames_.find(auth.nickname) != nicknames_.end()) {
                    response.result = false;
                    response.message = "Player already has this nickname";
                }
                auto response_str = serialize(response);
                session->push_message(response_str);
                if (!response.result)
                    session->close();
            }
        }
    }
}

} // namespace cosmo
