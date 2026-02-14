//
// Created by pointerlost on 1/25/26.
//
#pragma once
#include <Common/RealTypes.h>

namespace Real::physics {

    using RigidBodyHandle  = f32;
    using PhysicsShapeHandle = f32;
    constexpr RigidBodyHandle  InvalidRigidBodyHandle = 0;
    constexpr PhysicsShapeHandle InvalidShapeHandle = 0;

    enum class ColliderChangeType {
        Dirty,   // pose, size etc. update
        Rebuild, // shape or flags changed inside of component
        // (depending on what you use for library like Physx, Bullet etc.)
    };

    struct ColliderDebug {
        bool show = false; // per-collider override
        bool showBounds = false;
    };

    enum class BodyType {
        Static,
        Dynamic,
        Kinematic,
    };

    enum class ColliderShape {
        Box,
        Sphere,
        Capsule,
    };
}
