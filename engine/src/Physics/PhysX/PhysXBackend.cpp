//
// Created by pointerlost on 2/12/26.
//
#include <Physics/PhysX/PhysXBackend.h>
#include "PxPhysicsAPI.h"
#include "Core/Logger.h"
#include "Graphics/Debug/DebugRenderer.h"
#include "Math/Vec2.h"
#include "Physics/PhysicsUtils.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"

namespace {
    // PhysX-Real conversions
    using namespace Real;

    math::Vec2 FromPxVec(const physx::PxVec2& v) noexcept {
        return { v.x, v.y };
    }
    physx::PxVec2 ToPxVec(const math::Vec2& v) noexcept {
        return { v.x, v.y };
    }
    math::Vec3 FromPxVec(const physx::PxVec3& v) noexcept {
        return { v.x, v.y, v.z };
    }
    physx::PxVec3 ToPxVec(const math::Vec3& v) noexcept {
        return { v.x, v.y, v.z };
    }
    math::Vec4 FromPxVec(const physx::PxVec4& v) noexcept {
        return { v.x, v.y, v.z, v.w };
    }
    physx::PxVec4 ToPxVec(const math::Vec4& v) noexcept {
        return { v.x, v.y, v.z, v.w };
    }

    math::Quat FromPxQuat(const physx::PxQuat& q) noexcept {
        return { q.x, q.y, q.z, q.w };
    }
    physx::PxQuat ToPxQuat(const math::Quat& q) noexcept {
        return { q.x, q.y, q.z, q.w };
    }

    core::LocalPose FromPxTransform(const physx::PxTransform& t) noexcept {
        return { { FromPxVec(t.p) }, { FromPxQuat(t.q) } };
    }

    physx::PxTransform ToPxTransform(const core::LocalPose& t) noexcept {
        return { ToPxVec(t.position), ToPxQuat(t.rotation) };
    }
}

namespace Real::physics {

    void PhysXBackend::Init(const core::PhysicsWorldDesc &desc) {
        m_NextBodyHandle.id = 1.f;
        m_NextBodyHandle.id = 1.f;

        m_Context = CreateScope<PhysXContext>();
        // Initialize PhysX Foundation, Physics, Scene, etc.
        m_Context->Init();

        Info("PhysX initialized successfully!");
    }

    void PhysXBackend::Shutdown() {
        m_Context->Shutdown(); // Shutdown PhysX systems and release low-level resources
    }

    void PhysXBackend::Step(float dt) {
        auto& physXScene = m_Context->GetScene();

        // Start simulation step
        physXScene.simulate(dt);
        // Wait for simulation to finish and fetch results
        physXScene.fetchResults(true);
    }

    core::RigidBodyHandle PhysXBackend::CreateBody(const core::BodyDesc& bd)
    {
        // Generate unique body handle
        core::RigidBodyHandle handle = { m_NextBodyHandle.id++ };

        physx::PxRigidActor* actor = nullptr;

        switch (bd.type)
        {
            case core::BodyDesc::Type::Static:
                // Create non-movable rigid body
                actor = m_Context->GetPhysics().createRigidStatic(ToPxTransform(bd.localTransform));
                break;

            case core::BodyDesc::Type::Dynamic:
            {
                auto* dynamic = m_Context->GetPhysics().createRigidDynamic(ToPxTransform(bd.localTransform));
                dynamic->setAngularDamping(0.5f);
                actor = dynamic;
                break;
            }

            case core::BodyDesc::Type::Kinematic:
            {
                auto* dynamic = m_Context->GetPhysics().createRigidDynamic(ToPxTransform(bd.localTransform));
                dynamic->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
                actor = dynamic;
                break;
            }

            default: Warn("Actor is fucking nullptr! damnit");
        }

        // Add actor to PhysX scene
        m_Context->GetScene().addActor(*actor);

        // Store actor by handle
        m_bodies.emplace(handle, PhysXBody{ actor });
        return handle;
    }

    void PhysXBackend::DestroyBody(core::RigidBodyHandle rbh) {
        if (!m_bodies.contains(rbh))
            return;

        auto& body = m_bodies[rbh];

        m_Context->GetScene().removeActor(*body.actor);
        body.actor->release();

        m_bodies.erase(rbh);
    }

    core::ShapeHandle PhysXBackend::CreateShape(const core::ShapeDesc &sd) {
        // Generate unique shape handle
        core::ShapeHandle handle = { m_NextShapeHandle.id++ };

        physx::PxShape* shape = nullptr;
        auto& physics           = m_Context->GetPhysics();
        auto& defaultPxMaterial = m_Context->GetDefaultMaterial();

        switch (sd.shape)
        {
            case core::ShapeDesc::Shape::Box:
                shape = util::physics::CreatePhysXShapeFromReal(
                    physics,
                    &defaultPxMaterial,
                    core::ShapeDesc::Shape::Box
                );
                break;

            case core::ShapeDesc::Shape::Sphere:
                shape = util::physics::CreatePhysXShapeFromReal(
                    physics,
                    &defaultPxMaterial,
                    core::ShapeDesc::Shape::Sphere
                );
                break;

            case core::ShapeDesc::Shape::Capsule:
                shape = util::physics::CreatePhysXShapeFromReal(
                    physics,
                    &defaultPxMaterial,
                    core::ShapeDesc::Shape::Capsule
                );
                break;

            default: {
                Warn("[PhysXBackend::CreateShape] There is no shape with this type damnit!");
            }
        }

        if (sd.isTrigger) {
            // Disable physical collision response
            shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);

            // Enable trigger behavior (overlap events only)
            shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);
        }

        if (!shape)
            return core::ShapeHandle{}; // returning 0 which is

        m_shapes.emplace(handle, PhysXShape{ shape });
        return handle;
    }

    void PhysXBackend::DestroyShape(core::ShapeHandle sh) {
        if (!m_shapes.contains(sh))
            return;

        auto& shape = m_shapes[sh];

        // Release PhysX shape
        shape.shape->release();

        m_shapes.erase(sh);
    }

    void PhysXBackend::AttachShape(core::RigidBodyHandle rbh, core::ShapeHandle sh) {
        if (!m_bodies.contains(rbh) || !m_shapes.contains(sh))
            return;

        auto* actor = m_bodies[rbh].actor;
        auto* shape = m_shapes[sh].shape;

        // Attach shape to actor
        actor->attachShape(*shape);
    }

    void PhysXBackend::DetachShape(core::RigidBodyHandle rbh, core::ShapeHandle sh) {
        if (!m_bodies.contains(rbh) || !m_shapes.contains(sh))
            return;

        auto* actor = m_bodies[rbh].actor;
        auto* shape = m_shapes[sh].shape;

        // Detach shape from actor
        actor->detachShape(*shape);
    }

    void PhysXBackend::SetShapeLocalTransform(
        core::ShapeHandle handle,
        const math::Vec3 &position,
        const math::Quat &rotation)
    {
        if (!m_shapes.contains(handle))
            return;

        auto* shape = m_shapes[handle].shape;

        // Set local pose relative to owning actor
        shape->setLocalPose(physx::PxTransform(ToPxVec(position), ToPxQuat(rotation)));
    }

    void PhysXBackend::SetShapeEnabled(core::ShapeHandle handle, bool enabled) {
        if (!m_shapes.contains(handle))
            return;

        auto* shape = m_shapes[handle].shape;

        // Check if shape is a trigger
        const bool isTrigger = shape->getFlags() & physx::PxShapeFlag::eTRIGGER_SHAPE;

        // Do not modify simulation flag for triggers (PhysX don't accept this!!)
        if (isTrigger)
            return; // triggers should not toggle simulation

        // Enable or disable physical collision response
        shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, enabled);
    }

    void PhysXBackend::SetBodyTransform(core::RigidBodyHandle handle, const core::LocalPose &t) {
        if (!m_bodies.contains(handle))
            return;

        // Set world transform of actor
        m_bodies[handle].actor->setGlobalPose(ToPxTransform(t));
    }

    core::LocalPose PhysXBackend::GetBodyTransform(core::RigidBodyHandle handle) const {
        if (!m_bodies.contains(handle))
            return {};

        // Read world transform from actor
        return FromPxTransform(m_bodies.at(handle).actor->getGlobalPose());
    }

    physx::PxRigidStatic* PhysXBackend::CreateStaticActor(const core::LocalPose& t) const {
        auto* sActor = m_Context->GetPhysics().createRigidStatic(ToPxTransform(t));
        return sActor;
    }

    physx::PxRigidDynamic* PhysXBackend::CreateDynamicActor(const core::LocalPose& t) const {
        auto* dynamic = m_Context->GetPhysics().createRigidDynamic(ToPxTransform(t));
        // set object's rotation speed (angular damping)
        dynamic->setAngularDamping(0.5f);
        return dynamic;
    }

    physx::PxRigidDynamic* PhysXBackend::CreateKinematicActor(const core::LocalPose& t) const {
        auto* kinematicBody = m_Context->GetPhysics().createRigidDynamic(ToPxTransform(t));
        kinematicBody->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
        return kinematicBody;
    }

    PhysXBody PhysXBackend::GetPxRigidActor(core::RigidBodyHandle rbHandle) {
        return m_bodies.contains(rbHandle) ? m_bodies[rbHandle] : PhysXBody{};
    }

    PhysXShape PhysXBackend::GetPxShape(core::ShapeHandle shapeHandle) {
        return m_shapes.contains(shapeHandle) ? m_shapes[shapeHandle] : PhysXShape{};
    }
}
