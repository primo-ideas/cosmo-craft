# By Claude Opus

project(
  'jolt',
  'cpp',
  version : '5.6.0',
  license : 'MIT',
  meson_version : '>= 1.3.0',
  default_options : [
    'cpp_std=c++17',
  ],
)

cpp = meson.get_compiler('cpp')
is_msvc = cpp.get_argument_syntax() == 'msvc'
is_x86 = host_machine.cpu_family() in ['x86', 'x86_64']
deterministic = get_option('cross_platform_deterministic')

# Public arguments: they change Jolt's ABI, so the library and its consumers
# must see exactly the same ones. RegisterTypes() aborts at runtime when the
# feature bits of JPH_VERSION_ID differ, and mismatched instruction sets cause
# link errors.
jolt_args = [
  # server/meson.build adds -DJPH_NO_DEBUG for consumers, keep Jolt in sync
  '-DJPH_NO_DEBUG',
  '-DJPH_OBJECT_LAYER_BITS=' + get_option('object_layer_bits'),
]

feature_defines = {
  'double_precision' : 'JPH_DOUBLE_PRECISION',
  'cross_platform_deterministic' : 'JPH_CROSS_PLATFORM_DETERMINISTIC',
  'asserts' : 'JPH_ENABLE_ASSERTS',
  'debug_renderer' : 'JPH_DEBUG_RENDERER',
  'profiler' : 'JPH_PROFILE_ENABLED',
  'object_stream' : 'JPH_OBJECT_STREAM',
}
foreach opt, define : feature_defines
  if get_option(opt)
    jolt_args += '-D' + define
  endif
endforeach

# Instruction sets, same flags and defines as Jolt.cmake for each level
use_fmadd = false
if is_x86
  simd = get_option('simd')
  if simd == 'avx2'
    use_fmadd = not deterministic
    jolt_args += [
      '-DJPH_USE_SSE4_1',
      '-DJPH_USE_SSE4_2',
      '-DJPH_USE_AVX',
      '-DJPH_USE_AVX2',
      '-DJPH_USE_LZCNT',
      '-DJPH_USE_TZCNT',
      '-DJPH_USE_F16C',
    ]
    if use_fmadd
      jolt_args += '-DJPH_USE_FMADD'
    endif
    if is_msvc
      jolt_args += '/arch:AVX2'
    else
      jolt_args += ['-mavx2', '-mbmi', '-mpopcnt', '-mlzcnt', '-mf16c']
      if use_fmadd
        jolt_args += '-mfma'
      endif
    endif
  elif simd == 'sse4.2'
    jolt_args += ['-DJPH_USE_SSE4_1', '-DJPH_USE_SSE4_2']
    if not is_msvc
      jolt_args += ['-msse4.2', '-mpopcnt']
    endif
  elif not is_msvc
    jolt_args += '-msse2'
  endif
  if not is_msvc
    jolt_args += '-mfpmath=sse'
  endif
endif

# Private arguments, only used to compile Jolt itself
jolt_private_args = []
if not is_msvc
  # FMA contraction breaks determinism, and is only safe on x86 where Jolt's
  # FMA intrinsics are preserved
  if deterministic or (is_x86 and not use_fmadd)
    jolt_private_args += '-ffp-contract=off'
  elif is_x86
    jolt_private_args += '-ffp-contract=fast'
  else
    jolt_private_args += '-ffp-contract=on'
  endif
endif
if cpp.get_id() == 'gcc'
  # False positives that cannot be silenced from code, see Build/CMakeLists.txt
  jolt_private_args += cpp.get_supported_arguments('-Wno-stringop-overflow', '-Wno-psabi')
endif

# Source list copied from Jolt/Jolt.cmake (JOLT_PHYSICS_SRC_FILES), sync it
# when bumping the version. The compute backends (DX12, Vulkan, Metal, CPU)
# are left out.
jolt_sources = files(
  'Jolt/AABBTree/AABBTreeBuilder.cpp',
  'Jolt/Compute/ComputeSystem.cpp',
  'Jolt/Core/Color.cpp',
  'Jolt/Core/Factory.cpp',
  'Jolt/Core/IssueReporting.cpp',
  'Jolt/Core/JobSystemSingleThreaded.cpp',
  'Jolt/Core/JobSystemThreadPool.cpp',
  'Jolt/Core/JobSystemWithBarrier.cpp',
  'Jolt/Core/LinearCurve.cpp',
  'Jolt/Core/Memory.cpp',
  'Jolt/Core/Profiler.cpp',
  'Jolt/Core/RTTI.cpp',
  'Jolt/Core/Semaphore.cpp',
  'Jolt/Core/StringTools.cpp',
  'Jolt/Core/TickCounter.cpp',
  'Jolt/Geometry/ConvexHullBuilder.cpp',
  'Jolt/Geometry/ConvexHullBuilder2D.cpp',
  'Jolt/Geometry/Indexify.cpp',
  'Jolt/Geometry/OrientedBox.cpp',
  'Jolt/Math/Vec3.cpp',
  'Jolt/ObjectStream/SerializableObject.cpp',
  'Jolt/Physics/Body/Body.cpp',
  'Jolt/Physics/Body/BodyCreationSettings.cpp',
  'Jolt/Physics/Body/BodyInterface.cpp',
  'Jolt/Physics/Body/BodyManager.cpp',
  'Jolt/Physics/Body/MassProperties.cpp',
  'Jolt/Physics/Body/MotionProperties.cpp',
  'Jolt/Physics/Character/Character.cpp',
  'Jolt/Physics/Character/CharacterBase.cpp',
  'Jolt/Physics/Character/CharacterVirtual.cpp',
  'Jolt/Physics/Collision/BroadPhase/BroadPhase.cpp',
  'Jolt/Physics/Collision/BroadPhase/BroadPhaseBruteForce.cpp',
  'Jolt/Physics/Collision/BroadPhase/BroadPhaseQuadTree.cpp',
  'Jolt/Physics/Collision/BroadPhase/QuadTree.cpp',
  'Jolt/Physics/Collision/CastConvexVsTriangles.cpp',
  'Jolt/Physics/Collision/CastSphereVsTriangles.cpp',
  'Jolt/Physics/Collision/CollideConvexVsTriangles.cpp',
  'Jolt/Physics/Collision/CollideSphereVsTriangles.cpp',
  'Jolt/Physics/Collision/CollisionDispatch.cpp',
  'Jolt/Physics/Collision/CollisionGroup.cpp',
  'Jolt/Physics/Collision/EstimateCollisionResponse.cpp',
  'Jolt/Physics/Collision/GroupFilter.cpp',
  'Jolt/Physics/Collision/GroupFilterTable.cpp',
  'Jolt/Physics/Collision/ManifoldBetweenTwoFaces.cpp',
  'Jolt/Physics/Collision/NarrowPhaseQuery.cpp',
  'Jolt/Physics/Collision/NarrowPhaseStats.cpp',
  'Jolt/Physics/Collision/PhysicsMaterial.cpp',
  'Jolt/Physics/Collision/PhysicsMaterialSimple.cpp',
  'Jolt/Physics/Collision/Shape/BoxShape.cpp',
  'Jolt/Physics/Collision/Shape/CapsuleShape.cpp',
  'Jolt/Physics/Collision/Shape/CompoundShape.cpp',
  'Jolt/Physics/Collision/Shape/ConvexHullShape.cpp',
  'Jolt/Physics/Collision/Shape/ConvexShape.cpp',
  'Jolt/Physics/Collision/Shape/CylinderShape.cpp',
  'Jolt/Physics/Collision/Shape/DecoratedShape.cpp',
  'Jolt/Physics/Collision/Shape/EmptyShape.cpp',
  'Jolt/Physics/Collision/Shape/HeightFieldShape.cpp',
  'Jolt/Physics/Collision/Shape/MeshShape.cpp',
  'Jolt/Physics/Collision/Shape/MutableCompoundShape.cpp',
  'Jolt/Physics/Collision/Shape/OffsetCenterOfMassShape.cpp',
  'Jolt/Physics/Collision/Shape/PlaneShape.cpp',
  'Jolt/Physics/Collision/Shape/RotatedTranslatedShape.cpp',
  'Jolt/Physics/Collision/Shape/ScaledShape.cpp',
  'Jolt/Physics/Collision/Shape/Shape.cpp',
  'Jolt/Physics/Collision/Shape/SphereShape.cpp',
  'Jolt/Physics/Collision/Shape/StaticCompoundShape.cpp',
  'Jolt/Physics/Collision/Shape/TaperedCapsuleShape.cpp',
  'Jolt/Physics/Collision/Shape/TaperedCylinderShape.cpp',
  'Jolt/Physics/Collision/Shape/TriangleShape.cpp',
  'Jolt/Physics/Collision/TransformedShape.cpp',
  'Jolt/Physics/Constraints/ConeConstraint.cpp',
  'Jolt/Physics/Constraints/Constraint.cpp',
  'Jolt/Physics/Constraints/ConstraintManager.cpp',
  'Jolt/Physics/Constraints/ContactConstraintManager.cpp',
  'Jolt/Physics/Constraints/DistanceConstraint.cpp',
  'Jolt/Physics/Constraints/FixedConstraint.cpp',
  'Jolt/Physics/Constraints/GearConstraint.cpp',
  'Jolt/Physics/Constraints/HingeConstraint.cpp',
  'Jolt/Physics/Constraints/MotorSettings.cpp',
  'Jolt/Physics/Constraints/PathConstraint.cpp',
  'Jolt/Physics/Constraints/PathConstraintPath.cpp',
  'Jolt/Physics/Constraints/PathConstraintPathHermite.cpp',
  'Jolt/Physics/Constraints/PointConstraint.cpp',
  'Jolt/Physics/Constraints/PulleyConstraint.cpp',
  'Jolt/Physics/Constraints/RackAndPinionConstraint.cpp',
  'Jolt/Physics/Constraints/SixDOFConstraint.cpp',
  'Jolt/Physics/Constraints/SliderConstraint.cpp',
  'Jolt/Physics/Constraints/SpringSettings.cpp',
  'Jolt/Physics/Constraints/SwingTwistConstraint.cpp',
  'Jolt/Physics/Constraints/TwoBodyConstraint.cpp',
  'Jolt/Physics/DeterminismLog.cpp',
  'Jolt/Physics/Hair/Hair.cpp',
  'Jolt/Physics/Hair/HairSettings.cpp',
  'Jolt/Physics/Hair/HairShaders.cpp',
  'Jolt/Physics/Hair/RegisterHair.cpp',
  'Jolt/Physics/IslandBuilder.cpp',
  'Jolt/Physics/LargeIslandSplitter.cpp',
  'Jolt/Physics/PhysicsScene.cpp',
  'Jolt/Physics/PhysicsSystem.cpp',
  'Jolt/Physics/PhysicsUpdateContext.cpp',
  'Jolt/Physics/Ragdoll/Ragdoll.cpp',
  'Jolt/Physics/SoftBody/SoftBodyCreationSettings.cpp',
  'Jolt/Physics/SoftBody/SoftBodyMotionProperties.cpp',
  'Jolt/Physics/SoftBody/SoftBodyShape.cpp',
  'Jolt/Physics/SoftBody/SoftBodySharedSettings.cpp',
  'Jolt/Physics/StateRecorderImpl.cpp',
  'Jolt/Physics/Vehicle/MotorcycleController.cpp',
  'Jolt/Physics/Vehicle/TrackedVehicleController.cpp',
  'Jolt/Physics/Vehicle/VehicleAntiRollBar.cpp',
  'Jolt/Physics/Vehicle/VehicleCollisionTester.cpp',
  'Jolt/Physics/Vehicle/VehicleConstraint.cpp',
  'Jolt/Physics/Vehicle/VehicleController.cpp',
  'Jolt/Physics/Vehicle/VehicleDifferential.cpp',
  'Jolt/Physics/Vehicle/VehicleEngine.cpp',
  'Jolt/Physics/Vehicle/VehicleTrack.cpp',
  'Jolt/Physics/Vehicle/VehicleTransmission.cpp',
  'Jolt/Physics/Vehicle/Wheel.cpp',
  'Jolt/Physics/Vehicle/WheeledVehicleController.cpp',
  'Jolt/RegisterTypes.cpp',
  'Jolt/Renderer/DebugRenderer.cpp',
  'Jolt/Renderer/DebugRendererPlayback.cpp',
  'Jolt/Renderer/DebugRendererRecorder.cpp',
  'Jolt/Renderer/DebugRendererSimple.cpp',
  'Jolt/Skeleton/SkeletalAnimation.cpp',
  'Jolt/Skeleton/Skeleton.cpp',
  'Jolt/Skeleton/SkeletonMapper.cpp',
  'Jolt/Skeleton/SkeletonPose.cpp',
  'Jolt/TriangleSplitter/TriangleSplitter.cpp',
  'Jolt/TriangleSplitter/TriangleSplitterBinning.cpp',
  'Jolt/TriangleSplitter/TriangleSplitterMean.cpp',
)

if get_option('object_stream')
  jolt_sources += files(
    'Jolt/ObjectStream/ObjectStream.cpp',
    'Jolt/ObjectStream/ObjectStreamBinaryIn.cpp',
    'Jolt/ObjectStream/ObjectStreamBinaryOut.cpp',
    'Jolt/ObjectStream/ObjectStreamIn.cpp',
    'Jolt/ObjectStream/ObjectStreamOut.cpp',
    'Jolt/ObjectStream/ObjectStreamTextIn.cpp',
    'Jolt/ObjectStream/ObjectStreamTextOut.cpp',
    'Jolt/ObjectStream/TypeDeclarations.cpp',
  )
endif

jolt_inc = include_directories('.', is_system : true)
jolt_deps = [dependency('threads')]

jolt_lib = static_library(
  'Jolt',
  jolt_sources,
  include_directories : jolt_inc,
  cpp_args : jolt_args + jolt_private_args,
  dependencies : jolt_deps,
)

jolt_dep = declare_dependency(
  link_with : jolt_lib,
  include_directories : jolt_inc,
  compile_args : jolt_args,
  dependencies : jolt_deps,
  version : meson.project_version(),
)
