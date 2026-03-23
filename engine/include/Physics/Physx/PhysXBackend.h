//
// Created by pointerlost on 2/12/26.
//
#pragma once
#include "../../Core/IPhysicsBackend.h"
#include "Physics/PhysicsTypes.h"
#include "Physics/PhysXContext.h"
#include <unordered_map>
#include "PhysXTypes.h"

namespace Real {
    struct TransformComponent;
    class Scene;
    namespace physics {
        enum class BodyType;
    }
}

namespace Real::physics {

    class PhysXBackend final : public core::IPhysicsBackend {
    public:
        void Init(const PhysicsWorldDesc &desc) override;
        void Shutdown() override;
        void Step(float dt) override;

        RigidBodyHandle CreateBody(const BodyDesc &bd) override;
        void DestroyBody(RigidBodyHandle handle) override;

        PhysicsShapeHandle CreateShape(const ShapeDesc & sd) override;
        void DestroyShape(PhysicsShapeHandle handle) override;

        void AttachShape(RigidBodyHandle rb, PhysicsShapeHandle ps) override;
        void DetachShape(RigidBodyHandle rb, PhysicsShapeHandle ps) override;
        void SetShapeLocalTransform(PhysicsShapeHandle handle,
            const math::Vec3 &position, const math::Quat &rotation
        ) override;
        void SetShapeEnabled(PhysicsShapeHandle handle, bool enabled) override;

        void SetBodyTransform(RigidBodyHandle handle, const Transform& t) override;
        Transform GetBodyTransform(RigidBodyHandle handle) const override;

    private:
        physx::PxRigidStatic  *CreateStaticActor(const Transform &t);
        physx::PxRigidDynamic *CreateDynamicActor(const Transform &t);
        physx::PxRigidDynamic *CreateKinematicActor(const Transform &t);

        PhysXBody GetPxRigidActor(RigidBodyHandle rbHandle);
        PhysXShape GetPxShape(PhysicsShapeHandle shapeHandle);

    private:
        Scope<PhysXContext> m_Context;

        RigidBodyHandle m_NextBodyHandle = 1;
        PhysicsShapeHandle m_NextShapeHandle = 1;
        std::unordered_map<RigidBodyHandle, PhysXBody> m_bodies;
        std::unordered_map<PhysicsShapeHandle, PhysXShape> m_shapes;
    };
}