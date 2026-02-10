//
// Created by pointerlost on 1/25/26.
//
#pragma once

namespace Real::physics {

    enum class ColliderChangeType {
        Dirty,   // pose, size etc. update
        Rebuild, // shape or flags changed inside of component
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
