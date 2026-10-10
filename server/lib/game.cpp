#include "game.hpp"

#include <Jolt/Jolt.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/RegisterTypes.h>

#include <algorithm>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "cosmo.hpp"
#include "player.hpp"
#include "session.hpp"

using nlohmann::json;

namespace cosmo {

void Game::clean_sessions() {
    {
        std::lock_guard lock(sessions_mutex_);
        auto            result = std::remove_if(in_sessions_.begin(), in_sessions_.end(),
                                                [](auto s) { return s->is_closing(); });
    }
    auto iter     = players_.begin();
    auto end_iter = players_.end();
    for (; iter != end_iter;) {
        if (iter->second->is_closing()) {
            iter = players_.erase(iter);
        } else {
            ++iter;
        }
    }
}

void Game::handle_in_sessions() {
    auto to_remove = std::vector<std::shared_ptr<Session>>();
    for (auto in_session : in_sessions_) {
        auto         messages = in_session->pop_messages();
        AuthResponse response;
        if (messages.size() > 0 && messages.size() != 1)
            response = AuthResponse{false, "Two many messages for auth"};
        else if (messages.size() > 0) {
            auto maybe_auth = deserialize<ClientAuth>(messages.back());
            if (!maybe_auth)
                response = AuthResponse{false, "Invalid data for auth"};
            else {
                auto auth = maybe_auth.value();
                if (is_player_online(auth.nickname)) {
                    response = AuthResponse{false, "Player already has this nickname"};
                } else {
                    players_[auth.nickname] = std::make_shared<Player>(in_session);
                    to_remove.push_back(in_session);
                }
            }
        }
        auto response_str = serialize(response);
        in_session->push_message(response_str);
        if (!response.result) {
            in_session->close();
        }
        in_session->set_authenticated();
    }

    for (auto in_session_to_remove : to_remove)
        in_sessions_.erase(
            std::find(in_sessions_.cbegin(), in_sessions_.cend(), in_session_to_remove));
}

void Game::handle_actions() {
    for (auto pair : players_) {
        auto player   = pair.second;
        auto session  = pair.second->session();
        auto messages = session->pop_messages();
        for (auto msg : messages) {
            if (!player->is_waiting_for_action()) {
                auto maybe_client_next = deserialize<ClientNext>(msg);
                if (!maybe_client_next) {
                    session->close();
                    break;
                }
                auto client_next = maybe_client_next.value();
                player->set_next_action(client_next.action_type);
                player->wait_for_action();
            } else {
                auto next_action = player->get_next_action();
                if (next_action == NextAction::Move) {
                    auto maybe_move = deserialize<ActionMove>(msg);
                    if (!maybe_move) {
                        session->close();
                        break;
                    }
                    auto move = maybe_move.value();
                    player->prepare_move(move.dir_x, move.dir_y, move.dir_z);
                } else {
                    session->close();
                    break;
                }
            }
        }
    }
}

void Game::handle_physics() {
    physics_.step();
}

void Game::cycle() {
    clean_sessions();
    handle_in_sessions();
    handle_actions();
    handle_physics();
}

void Game::new_session(std::shared_ptr<Session> session) {
    sessions_mutex_.lock();
    in_sessions_.push_back(session);
    sessions_mutex_.unlock();
    session->run();
}

bool Game::is_player_online(std::string const &nickname) {
    return players_.contains(nickname);
}

// void Game::add_new_player(std::string const &nickname) {

//     nicknames_.insert(nickname);
// }

} // namespace cosmo
