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

    physics_system_.Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints,
                         broad_phase_layer_interface_, object_vs_broadphase_layer_filter_,
                         object_vs_object_layer_filter_);
    physics_system_.SetBodyActivationListener(&body_activation_listener_);
    physics_system_.SetContactListener(&contact_listener_);
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
    physics_system_.Update(1. / 10, 1, &*temp_allocator_, &*job_system_);
}

JPH::BodyInterface &Physics::get_body_interface() {
    return physics_system_.GetBodyInterface();
}

} // namespace cosmo
