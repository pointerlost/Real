//
// Created by pointerlost on 2/12/26.
//
#pragma once
#include "PhysicsTypes.h"

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

namespace Real::physics {

    class IPhysicsBackend {
    public:
        virtual ~IPhysicsBackend() = default;
        virtual void Init(const PhysicsWorldDesc&) = 0;
        virtual void Shutdown() = 0;
        virtual void Step(float) = 0;

        virtual RigidBodyHandle CreateBody(const BodyDesc&) = 0;
        virtual void DestroyBody(RigidBodyHandle) = 0;

        virtual PhysicsShapeHandle CreateShape(const ShapeDesc&) = 0;
        virtual void DestroyShape(PhysicsShapeHandle) = 0;

        virtual void AttachShape(RigidBodyHandle, PhysicsShapeHandle) = 0;
        virtual void DetachShape(RigidBodyHandle, PhysicsShapeHandle) = 0;
        virtual void SetShapeLocalTransform(PhysicsShapeHandle, const math::Vec3&, const math::Quat&) = 0;
        virtual void SetShapeEnabled(PhysicsShapeHandle, bool) = 0;

        virtual void SetBodyTransform(RigidBodyHandle, const Transform&) = 0;
        virtual Transform GetBodyTransform(RigidBodyHandle) const = 0;
    };
}