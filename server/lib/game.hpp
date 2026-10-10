#pragma once

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

#include <expected>
#include <map>
#include <memory>

#include "cosmo.hpp"
#include "physics.hpp"

#include <boost/asio/io_context.hpp>
#include <nlohmann/json.hpp>

namespace cosmo {

class Session;
class Player;

enum class AuthenticationError {
    InvalidData,
    NicknameTaken,

};

class Game : public std::enable_shared_from_this<Game> {
  private:
    std::map<std::string, std::shared_ptr<Player>> players_;
    std::vector<std::shared_ptr<Session>>          in_sessions_;
    std::mutex                                     sessions_mutex_;
    Physics                                        physics_;

  public:
    void                                             clean_sessions();
    void                                             handle_in_sessions();
    void                                             handle_actions();
    void                                             handle_physics();
    std::expected<AuthResponse, AuthenticationError> authentication(std::string const &auth);
    void                                             cycle();
    void read_messages(std::vector<std::shared_ptr<Session>>  sessions,
                       std::vector<std::shared_ptr<Session>> &sessions_to_remove);
    void new_session(std::shared_ptr<Session>);
    bool is_player_online(std::string const &);
    void add_new_player(std::string const &);
};

} // namespace cosmo
