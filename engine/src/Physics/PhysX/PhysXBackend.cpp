//
// Created by pointerlost on 2/12/26.
//
#include <Physics/Physx/PhysXBackend.h>

#include "Core/Logger.h"
#include "Graphics/Debug/DebugRenderer.h"
#include "Math/Vec2.h"
#include "Physics/PhysicsDescriptors.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"
#include "Util/Util.h"

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

    Transform FromPxTransform(const physx::PxTransform& t) noexcept {
        return { { FromPxVec(t.p) }, { FromPxQuat(t.q) } };
    }

    physx::PxTransform ToPxTransform(const Transform& t) noexcept {
        return { ToPxVec(t.position), ToPxQuat(t.rotation) };
    }
}

namespace Real::physics {

    void PhysXBackend::Init(const PhysicsWorldDesc &desc) {
        m_Context = CreateScope<PhysXContext>();
        // Initialize PhysX Foundation, Physics, Scene, etc.
        m_Context->Init();

        Info("PhysX initialized successfully!");
    }

    void PhysXBackend::Shutdown() {
        m_Context->Shutdown(); // Shutdown PhysX systems and release low-level resources
    }

    void PhysXBackend::Step(float deltaTime) {
        auto& physXScene = m_Context->GetScene();

        // Start simulation step
        physXScene.simulate(deltaTime);
        // Wait for simulation to finish and fetch results
        physXScene.fetchResults(true);
    }

    RigidBodyHandle PhysXBackend::CreateBody(const BodyDesc &bd) {
        // Generate unique body handle
        RigidBodyHandle handle = m_NextBodyHandle++;

        physx::PxRigidActor* actor = nullptr;

        switch (bd.type)
        {
            case BodyType::Static:
                // Create non-movable rigid body
                actor = m_Context->GetPhysics().createRigidStatic(ToPxTransform(bd.worldTransform));
                break;

            case BodyType::Dynamic:
            {
                auto* dynamic = m_Context->GetPhysics().createRigidDynamic(ToPxTransform(bd.worldTransform));
                dynamic->setAngularDamping(0.5f);
                actor = dynamic;
                break;
            }

            case BodyType::Kinematic:
            {
                auto* dynamic = m_Context->GetPhysics().createRigidDynamic(ToPxTransform(bd.worldTransform));
                dynamic->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
                actor = dynamic;
                break;
            }
        }

        // Add actor to PhysX scene
        m_Context->GetScene().addActor(*actor);

        // Store actor by handle
        m_bodies.emplace(handle, PhysXBody{ actor });
        return handle;
    }

    void PhysXBackend::DestroyBody(RigidBodyHandle handle) {
        if (!m_bodies.contains(handle))
            return;

        auto& body = m_bodies[handle];

        m_Context->GetScene().removeActor(*body.actor);
        body.actor->release();

        m_bodies.erase(handle);
    }

    PhysicsShapeHandle PhysXBackend::CreateShape(const ShapeDesc &sd) {
        // Generate unique shape handle
        PhysicsShapeHandle handle = m_NextShapeHandle++;

        physx::PxShape* shape = nullptr;
        auto& physics = m_Context->GetPhysics();
        auto& defaultPxMaterial = m_Context->GetDefaultMaterial();

        switch (sd.shape)
        {
            case ColliderShape::Box:
                shape = util::CreatePhysXShapeFromReal(
                    physics,
                    &defaultPxMaterial,
                    ColliderShape::Box
                );
                break;

            case ColliderShape::Sphere:
                shape = util::CreatePhysXShapeFromReal(
                    physics,
                    &defaultPxMaterial,
                    ColliderShape::Sphere
                );
                break;

            case ColliderShape::Capsule:
                shape = util::CreatePhysXShapeFromReal(
                    physics,
                    &defaultPxMaterial,
                    ColliderShape::Capsule
                );
                break;

            default: ;
        }

        if (sd.isTrigger) {
            // Disable physical collision response
            shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);

            // Enable trigger behavior (overlap events only)
            shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);
        }

        if (!shape)
            return InvalidShapeHandle;

        m_shapes.emplace(handle, PhysXShape{ shape });
        return handle;
    }

    void PhysXBackend::DestroyShape(PhysicsShapeHandle handle) {
        if (!m_shapes.contains(handle))
            return;

        auto& shape = m_shapes[handle];

        // Release PhysX shape
        shape.shape->release();

        m_shapes.erase(handle);
    }

    void PhysXBackend::AttachShape(RigidBodyHandle rb, PhysicsShapeHandle ps) {
        if (!m_bodies.contains(rb) || !m_shapes.contains(ps))
            return;

        auto* actor = m_bodies[rb].actor;
        auto* shape = m_shapes[ps].shape;

        // Attach shape to actor
        actor->attachShape(*shape);
    }

    void PhysXBackend::DetachShape(RigidBodyHandle rb, PhysicsShapeHandle ps) {
        if (!m_bodies.contains(rb) || !m_shapes.contains(ps))
            return;

        auto* actor = m_bodies[rb].actor;
        auto* shape = m_shapes[ps].shape;

        // Detach shape from actor
        actor->detachShape(*shape);
    }

    void PhysXBackend::SetShapeLocalTransform(PhysicsShapeHandle handle,
        const math::Vec3 &position, const math::Quat &rotation)
    {
        if (!m_shapes.contains(handle))
            return;

        auto* shape = m_shapes[handle].shape;

        // Set local pose relative to owning actor
        shape->setLocalPose(physx::PxTransform(ToPxVec(position), ToPxQuat(rotation)));
    }

    void PhysXBackend::SetShapeEnabled(PhysicsShapeHandle handle, bool enabled) {
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

    void PhysXBackend::SetBodyTransform(RigidBodyHandle handle, const Transform &t) {
        if (!m_bodies.contains(handle))
            return;

        // Set world transform of actor
        m_bodies[handle].actor->setGlobalPose(ToPxTransform(t));
    }

    Transform PhysXBackend::GetBodyTransform(RigidBodyHandle handle) const {
        if (!m_bodies.contains(handle))
            return {};

        // Read world transform from actor
        return FromPxTransform(m_bodies.at(handle).actor->getGlobalPose());
    }

    physx::PxRigidStatic* PhysXBackend::CreateStaticActor(const Transform &t) {
        auto* sActor = m_Context->GetPhysics().createRigidStatic(ToPxTransform(t));
        return sActor;
    }

    physx::PxRigidDynamic* PhysXBackend::CreateDynamicActor(const Transform &t) {
        auto* dynamic = m_Context->GetPhysics().createRigidDynamic(ToPxTransform(t));
        // set object's rotation speed (angular damping)
        dynamic->setAngularDamping(0.5f);
        return dynamic;
    }

    physx::PxRigidDynamic* PhysXBackend::CreateKinematicActor(const Transform &t) {
        auto* kinematicBody = m_Context->GetPhysics().createRigidDynamic(ToPxTransform(t));
        kinematicBody->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
        return kinematicBody;
    }

    PhysXBody PhysXBackend::GetPxRigidActor(RigidBodyHandle rbHandle) {
        return m_bodies.contains(rbHandle) ? m_bodies[rbHandle] : PhysXBody{};
    }

    PhysXShape PhysXBackend::GetPxShape(PhysicsShapeHandle shapeHandle) {
        return m_shapes.contains(shapeHandle) ? m_shapes[shapeHandle] : PhysXShape{};
    }
}
