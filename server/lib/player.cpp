#include "player.hpp"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>

#include <memory>

#include "Jolt/Physics/Collision/Shape/CapsuleShape.h"
#include "physics.hpp"
#include "session.hpp"

using namespace JPH;
using namespace JPH::literals;

namespace cosmo {

Player::Player(BodyInterface &body_interface, int pos_x, int pos_y, int pos_z,
               std::shared_ptr<Session> session)
    : session_(session)
    , waiting_for_action_(false)
    , moving_(false) {

    BodyCreationSettings player_capsule_settings(new CapsuleShape(1.f, 0.1),
                                                 RVec3(pos_x, pos_y, pos_z), Quat::sIdentity(),
                                                 EMotionType::Dynamic, Layers::MOVING);
    player_capsule_id_ =
        body_interface.CreateAndAddBody(player_capsule_settings, EActivation::Activate);
    body_interface.SetLinearVelocity(player_capsule_id_, Vec3(0.0f, .0f, 0.0f));
}
bool Player::is_closing() const {
    return session_->is_closing();
}
std::shared_ptr<Session> Player::session() {
    return session_;
}

void Player::set_next_action(NextAction next_action) {
    next_action_ = next_action;
}

NextAction Player::get_next_action() const {
    return next_action_;
}

void Player::wait_for_action() {
    waiting_for_action_ = true;
}

bool Player::is_waiting_for_action() const {
    return waiting_for_action_;
}

void Player::move(JPH::BodyInterface &body_interface, float dir_x, float dir_y, float dir_z) {
    body_interface.SetLinearVelocity(player_capsule_id_, Vec3Arg(dir_x, dir_y, dir_z));
}

void Player::stop(JPH::BodyInterface &body_interface) {
    body_interface.SetLinearVelocity(player_capsule_id_, Vec3Arg());
}

} // namespace cosmo
