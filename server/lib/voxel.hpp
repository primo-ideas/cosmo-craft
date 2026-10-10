#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>

#include "Jolt/Physics/Body/BodyInterface.h"

namespace cosmo {

class Voxel {
  protected:
    JPH::BodyID physics_body_;
};

class DummyBlock : public Voxel {
  public:
    DummyBlock(JPH::BodyInterface &body_interface, int x, int y, int z);
};

} // namespace cosmo
