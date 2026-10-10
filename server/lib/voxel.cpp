#include "voxel.hpp"

#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>

#include "physics.hpp"

using namespace JPH;
using namespace JPH::literals;

namespace cosmo {

DummyBlock::DummyBlock(BodyInterface &body_interface, int x, int y, int z) {
    BoxShapeSettings block_shape_settings(Vec3(1.0f, 1.0f, 1.0f));
    // box_shape_settings.SetEmbedded();
    ShapeSettings::ShapeResult block_shape_result = block_shape_settings.Create();
    ShapeRefC                  block_shape        = block_shape_result.Get();
    BodyCreationSettings       block_body_settings(block_shape, RVec3(x, y, z), Quat::sIdentity(),
                                                   EMotionType::Static, Layers::NON_MOVING);
    Body                      *block = body_interface.CreateBody(block_body_settings);
    body_interface.AddBody(block->GetID(), EActivation::DontActivate);
    physics_body_ = block->GetID();
}

} // namespace cosmo
