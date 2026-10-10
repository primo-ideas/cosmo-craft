#pragma once

#include <memory>

#include "session.hpp"

namespace cosmo {

class Player {
  private:
    std::shared_ptr<Session> session_;
    NextAction               next_action_;
    bool                     waiting_for_action_;
    float                    dir_x_;
    float                    dir_y_;
    float                    dir_z_;
    bool                     moving_;

  public:
    Player(std::shared_ptr<Session> session);

    bool                     is_closing() const;
    std::shared_ptr<Session> session();
    void                     set_next_action(NextAction next_action);
    NextAction               get_next_action() const;
    void                     wait_for_action();
    bool                     is_waiting_for_action() const;
    void                     prepare_move(float dir_x, float dir_y, float dir_z);
    void                     move(float delta);
};

} // namespace cosmo
