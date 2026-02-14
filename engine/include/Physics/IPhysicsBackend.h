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
        virtual void Init(const PhysicsWorldDesc& desc) = 0;
        virtual void Shutdown() = 0;
        virtual void Step(float dt) = 0;

        virtual RigidBodyHandle CreateBody(const BodyDesc& bd) = 0;
        virtual void DestroyBody(RigidBodyHandle handle) = 0;

        virtual PhysicsShapeHandle CreateShape(const ShapeDesc& sd) = 0;
        virtual void DestroyShape(PhysicsShapeHandle handle) = 0;

        virtual void AttachShape(RigidBodyHandle rb, PhysicsShapeHandle ps) = 0;
        virtual void DetachShape(RigidBodyHandle rb, PhysicsShapeHandle ps) = 0;
        virtual void SetShapeLocalTransform(PhysicsShapeHandle handle,
            const math::Vec3& position, const math::Quat& rotation
        ) = 0;
        virtual void SetShapeEnabled(PhysicsShapeHandle handle, bool enabled) = 0;

        virtual void SetBodyTransform(RigidBodyHandle handle, const Transform& t) = 0;
        virtual Transform GetBodyTransform(RigidBodyHandle handle) const = 0;
    };
}