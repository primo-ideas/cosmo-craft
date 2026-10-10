#include "physics.hpp"

#include <Jolt/Jolt.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystem.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/RegisterTypes.h>

#include <cstdarg>
#include <iostream>
#include <memory>
#include <thread>

JPH_SUPPRESS_WARNINGS

using namespace JPH;

using namespace JPH::literals;

using namespace std;

static void TraceImpl(const char *inFMT, ...) {

    va_list list;
    va_start(list, inFMT);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), inFMT, list);
    va_end(list);

    std::cout << buffer << endl;
}

#ifdef JPH_ENABLE_ASSERTS

static bool AssertFailedImpl(const char *inExpression, const char *inMessage, const char *inFile,
                             uint inLine) {

    std::cout << inFile << ":" << inLine << ": (" << inExpression << ") "
              << (inMessage != nullptr ? inMessage : "") << endl;

    return true;
};

#endif

namespace cosmo {

Physics::Physics() {

    RegisterDefaultAllocator();

    temp_allocator_ = std::make_unique<TempAllocatorImpl>(10 * 1024 * 1024);
    auto nb_thr     = std::thread::hardware_concurrency();
    if (nb_thr == 0)
        nb_thr = 1;
    job_system_ =
        std::make_unique<JobSystemThreadPool>(cMaxPhysicsJobs, cMaxPhysicsBarriers, nb_thr);
    Trace = TraceImpl;
    JPH_IF_ENABLE_ASSERTS(AssertFailed = AssertFailedImpl;)

    Factory::sInstance = new Factory();

    RegisterTypes();

    const uint cMaxBodies             = 1024;
    const uint cNumBodyMutexes        = 0;
    const uint cMaxBodyPairs          = 1024;
    const uint cMaxContactConstraints = 1024;

    // BPLayerInterfaceImpl              broad_phase_layer_interface;
    // ObjectVsBroadPhaseLayerFilterImpl object_vs_broadphase_layer_filter;
    // ObjectLayerPairFilterImpl         object_vs_object_layer_filter;

    physics_system_.Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints,
                         broad_phase_layer_interface, object_vs_broadphase_layer_filter,
                         object_vs_object_layer_filter);

    // MyBodyActivationListener body_activation_listener;
    physics_system_.SetBodyActivationListener(&body_activation_listener);

    // MyContactListener contact_listener;
    physics_system_.SetContactListener(&contact_listener);

    // BodyInterface &body_interface = physics_system_.GetBodyInterface();

    // BoxShapeSettings floor_shape_settings(Vec3(100.0f, 1.0f, 100.0f));
    // floor_shape_settings.SetEmbedded();

    // ShapeSettings::ShapeResult floor_shape_result = floor_shape_settings.Create();
    // ShapeRefC                  floor_shape        = floor_shape_result.Get();

    // BodyCreationSettings floor_settings(floor_shape, RVec3(0.0_r, -1.0_r, 0.0_r),
    // Quat::sIdentity(),
    //                                     EMotionType::Static, Layers::NON_MOVING);

    // Body *floor = body_interface.CreateBody(floor_settings);

    // body_interface.AddBody(floor->GetID(), EActivation::DontActivate);

    // BodyCreationSettings sphere_settings(new SphereShape(0.5f), RVec3(0.0_r, 2.0_r, 0.0_r),
    //                                      Quat::sIdentity(), EMotionType::Dynamic,
    //                                      Layers::MOVING);
    // BodyID sphere_id = body_interface.CreateAndAddBody(sphere_settings, EActivation::Activate);

    // body_interface.SetLinearVelocity(sphere_id, Vec3(0.0f, -5.0f, 0.0f));

    // const float cDeltaTime = 1.0f / 60.0f;

    // physics_system_.OptimizeBroadPhase();
}

Physics::~Physics() {
    // body_interface.RemoveBody(sphere_id);

    // body_interface.DestroyBody(sphere_id);

    // body_interface.RemoveBody(floor->GetID());
    // body_interface.DestroyBody(floor->GetID());

    UnregisterTypes();

    delete Factory::sInstance;
    Factory::sInstance = nullptr;
}
void Physics::step() {

    // uint step = 0;
    // while (body_interface.IsActive(sphere_id)) {
    //     ++step;

    //     RVec3 position = body_interface.GetCenterOfMassPosition(sphere_id);
    //     Vec3  velocity = body_interface.GetLinearVelocity(sphere_id);
    //     // cout << "Step " << step << ": Position = (" << position.GetX() << ", " <<
    //     position.GetY()
    //     //      << ", " << position.GetZ() << "), Velocity = (" << velocity.GetX() << ", "
    //     //      << velocity.GetY() << ", " << velocity.GetZ() << ")" << endl;

    //     const int cCollisionSteps = 1;

    physics_system_.Update(1. / 10, 1, &*temp_allocator_, &*job_system_);
    // }
}

} // namespace cosmo
