#include "player.hpp"

#include <memory>

#include "session.hpp"

namespace cosmo {

Player::Player(std::shared_ptr<Session> session)
    : session_(session)
    , waiting_for_action_(false)
    , dir_x_(0)
    , dir_y_(0)
    , dir_z_(-1)
    , moving_(false) {
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

void Player::prepare_move(float dir_x, float dir_y, float dir_z) {
    dir_x_  = dir_x;
    dir_y_  = dir_y;
    dir_z_  = dir_z;
    moving_ = true;
}

void Player::move(float delta) {

    moving_ = false;
}

} // namespace cosmo
