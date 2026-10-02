#include "game.hpp"

#include <expected>

#include "net.hpp"

// namespace asio = boost::asio;

using nlohmann::json;

namespace cosmo {

void run_game(std::shared_ptr<Listener> listener) {
    while (true) {
        auto sessions = listener->sessions();

        for (auto &session : sessions) {
            std::expected<std::string, Session::pop_message_error> maybe_msg;
            while ((maybe_msg = session->pop_message())) {
                auto msg = maybe_msg.value();
                auto auth = json::parse(msg).get<ClientAuth>();
            }
        }
    }
    // json j = ClientAuth{"Primo"};
    // std::string s = j.dump();

    // auto auth = json::parse(s).get<ClientAuth>();
}

} // namespace cosmo
