//
// Created by pointerlost on 2/12/26.
//
#pragma once
#include <unordered_map>
#include "PhysXContext.h"
#include "PhysXTypes.h"
#include "Core/IPhysicsBackend.h"

namespace physx {
    class PxRigidDynamic;
    class PxRigidStatic;
}

namespace Real::physics {

    class PhysXBackend final : public core::IPhysicsBackend {
    public:
        void Init(const core::PhysicsWorldDesc& desc) override;
        void Shutdown()                               override;
        void Step(float dt)                           override;

        core::RigidBodyHandle CreateBody(const core::BodyDesc& bd) override;
        void DestroyBody(core::RigidBodyHandle rbh)                override;
        core::ShapeHandle CreateShape(const core::ShapeDesc& sd)   override;
        void DestroyShape(core::ShapeHandle sh)                    override;

        void AttachShape(core::RigidBodyHandle rbh, core::ShapeHandle sh)          override;
        void DetachShape(core::RigidBodyHandle rbh, core::ShapeHandle sh)          override;
        void SetBodyTransform(core::RigidBodyHandle rbh, const core::LocalPose &t) override;
        core::LocalPose GetBodyTransform(core::RigidBodyHandle rbh)          const override;

    private:
        physx::PxRigidStatic  *CreateStaticActor   (const core::LocalPose &t) const;
        physx::PxRigidDynamic *CreateDynamicActor  (const core::LocalPose &t) const;
        physx::PxRigidDynamic *CreateKinematicActor(const core::LocalPose &t) const;

        PhysXBody  GetPxRigidActor(core::RigidBodyHandle rbHandle);
        PhysXShape GetPxShape(core::ShapeHandle shapeHandle);

    private:
        Scope<PhysXContext> m_Context;

        core::RigidBodyHandle m_NextBodyHandle{};
        core::ShapeHandle     m_NextShapeHandle{};

        std::unordered_map<core::RigidBodyHandle, PhysXBody>  m_bodies;
        std::unordered_map<core::ShapeHandle,     PhysXShape> m_shapes;
    };
}