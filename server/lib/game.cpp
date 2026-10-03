#include "game.hpp"

#include <expected>

#include "log.hpp"
#include "net.hpp"

// namespace asio = boost::asio;

using nlohmann::json;

namespace cosmo {

void run_game(std::shared_ptr<Listener> listener) {
    while (true) {
        auto sessions = listener->sessions();

        for (auto &session : sessions) {
            auto messages = session->pop_messages();
            for (auto msg : messages) {
                auto parse_result = json::parse(msg, nullptr, false);
                if (parse_result.is_discarded()) {
                    logger.trace("Invalid JSON");
                }
                try {
                    auto auth = parse_result.get<ClientAuth>();
                } catch (...) {
                    logger.trace("Invalid input");
                }
            }
        }
    }
}

}

} // namespace cosmo
