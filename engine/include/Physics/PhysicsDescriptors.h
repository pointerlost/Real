//
// Created by pointerlost on 2/12/26.
//
#pragma once
#include "PhysicsTypes.h"
#include "Graphics/Transformations.h"

namespace Real::physics {
    enum class ColliderShape;
    enum class BodyType;
}

namespace Real::physics {

    struct PhysicsWorldDesc {
        math::Vec3 gravity{0.f, -9.81f, 0.f};
    };

    struct BodyDesc {
        BodyType type = BodyType::Static;
        float mass = 1.0f;
        Transform worldTransform;
    };

    struct ShapeDesc {
        ColliderShape shape = ColliderShape::Box;
        math::Vec3 size{0.5f};
        math::Vec3 localPosition;
        math::Quat localRotation;
        bool isTrigger{};
    };
}