//
// Created by pointerlost on 1/25/26.
//
#pragma once

namespace Real::physics {

    enum class PhysicsBackend {
        PhysX,
    };

    enum class ColliderChangeType {
        Dirty,   // pose, size etc. update
        Rebuild, // shape or flags changed inside of component
        // (depending on what we use for library like Physx, Bullet etc.)
    };

    struct ColliderDebug {
        bool show = false; // per-collider override
        bool showBounds = false;
    };
}
