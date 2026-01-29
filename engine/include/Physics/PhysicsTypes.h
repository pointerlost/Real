//
// Created by pointerlost on 1/25/26.
//
#pragma once

namespace Real::physics {

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
