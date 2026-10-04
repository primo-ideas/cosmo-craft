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
                auto maybe_auth = deserialize<ClientAuth>(msg);
                if (!maybe_auth.has_value()) {
                    logger.trace("Serde error: {}", (int)maybe_auth.error());
                    response.result = false;
                    session->close();
                }
                auto auth = maybe_auth.value();
                if (nicknames_.find(auth.nickname) != nicknames_.end()) {
                    response.result = false;
                    session->close();
                }
                auto response_str = serialize(response);
                session->push_message(response_str);
            }
        }
    }
}

} // namespace cosmo
