//
// Created by pointerlost on 2/12/26.
//
#pragma once
#include "../Physics/PhysicsTypes.h"

namespace Real {
    namespace math {
        struct Quat;
        struct Vec3;
    }

    class Entity;
    class Scene;
    struct Transform;

    namespace physics {
        struct PhysicsWorldDesc;
        struct ShapeDesc;
        struct BodyDesc;
    }
}

namespace Real::core {

    class IPhysicsBackend {
    public:
        virtual ~IPhysicsBackend() = default;
        virtual void Init(const physics::PhysicsWorldDesc&) = 0;
        virtual void Shutdown() = 0;
        virtual void Step(float) = 0;

        virtual physics::RigidBodyHandle CreateBody(const physics::BodyDesc&) = 0;
        virtual void DestroyBody(physics::RigidBodyHandle) = 0;

        virtual physics::PhysicsShapeHandle CreateShape(const physics::ShapeDesc&) = 0;
        virtual void DestroyShape(physics::PhysicsShapeHandle) = 0;

        virtual void AttachShape(physics::RigidBodyHandle, physics::PhysicsShapeHandle) = 0;
        virtual void DetachShape(physics::RigidBodyHandle, physics::PhysicsShapeHandle) = 0;
        virtual void SetShapeLocalTransform(physics::PhysicsShapeHandle, const math::Vec3&, const math::Quat&) = 0;
        virtual void SetShapeEnabled(physics::PhysicsShapeHandle, bool) = 0;

        virtual void SetBodyTransform(physics::RigidBodyHandle, const Transform&) = 0;
        virtual Transform GetBodyTransform(physics::RigidBodyHandle) const = 0;
    };
}