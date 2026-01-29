//
// Created by pointerlost on 1/14/26.
//
#include <Scene/Systems/PhysicsSystem.h>
#include "Core/Logger.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"
#include "Scene/Scene.h"
#include "Util/Util.h"

namespace Real::ecs {
    using namespace physx;

    void PhysicsSystem::Init() {
        m_PhysX = CreateScope<physics::PhysXContext>();
        // Init resources of PhysX
        m_PhysX->Init();

        Info("PhysX initialized successfully!");
    }

    void PhysicsSystem::Update(Scene *scene, float deltaTime) {
        auto& physXScene = m_PhysX->GetScene();

        // Step PhysX
        physXScene.simulate(deltaTime);
        physXScene.fetchResults(true);

        const auto view = m_Registry->view<TransformComponent, PhysicsBodyComponent, ColliderComponent>();

        for (const auto entity : view) {
            auto& tc = view.get<TransformComponent>(entity);
            const auto& pb = view.get<PhysicsBodyComponent>(entity);
            const auto& cc = view.get<ColliderComponent>(entity);

            if (pb.bodyType != physics::BodyType::Dynamic)
                continue;

            const auto* actor = cc.actor->is<PxRigidDynamic>();
            if (!actor) continue;

            const PxTransform pxT = actor->getGlobalPose();
            tc.transform = util::PXToReal(pxT);
        }
    }

    void PhysicsSystem::Shutdown() {
        m_PhysX->Shutdown(); // PhysX lifetime and low-level states like Physics, Foundation(PhysX) etc.
    }

    void PhysicsSystem::SetRegistry(entt::registry &registry) {
        m_Registry = &registry;
    }

    void PhysicsSystem::OnRigidBodyAdded(Entity e, Scene *scene) {
        SyncPhysics(e);
    }

    void PhysicsSystem::OnRigidBodyChanged(Entity e, Scene *scene) {
    }

    void PhysicsSystem::OnColliderAdded(Entity e, Scene *scene) {
        auto& cc = e.GetComponentUnchecked<ColliderComponent>();
        CreateColliderShape(cc);

        if (!cc.actor) {
            CreateBody(e); // Creates an actor based on body type
        }

        AttachCollider(cc.actor, cc.shapeHandle);
        FinalizeActor(e);
    }

    void PhysicsSystem::OnColliderChanged(Entity e, Scene *scene) {
    }

    PxRigidActor* PhysicsSystem::CreateBody(Entity& entity) {
        const auto& tc = entity.GetComponentUnchecked<TransformComponent>();
        auto& cc = entity.GetComponentUnchecked<ColliderComponent>();
        const auto pbc = entity.GetComponentForModification<PhysicsBodyComponent>();
        const auto bodyType = pbc ? pbc->bodyType : physics::BodyType::Static;

        PxRigidActor* actor = nullptr;

        switch (bodyType) {
            case physics::BodyType::Static:
                actor = CreateStaticActor(tc);
                break;
            case physics::BodyType::Kinematic:
                actor = CreateKinematicActor(tc);
                break;
            case physics::BodyType::Dynamic:
                actor = CreateDynamicActor(tc);
        }

        cc.actor = actor;
        return cc.actor;
    }

    PxShape* PhysicsSystem::CreateColliderShape(ColliderComponent& cc) {
        cc.shapeHandle = util::CreatePhysXShapeFromReal(m_PhysX->GetPhysics(), &m_PhysX->GetDefaultMaterial(), cc.shape);
        return cc.shapeHandle;
    }

    void PhysicsSystem::AttachCollider(PxRigidActor* actor, PxShape* shape) {
        actor->attachShape(*shape);
    }

    void PhysicsSystem::SyncPhysics(Entity e) {
        if (!e.HasComponent<ColliderComponent>())
            return;

        const physics::BodyType type = e.HasComponent<PhysicsBodyComponent>()
                ? e.GetComponentUnchecked<PhysicsBodyComponent>().bodyType
                : physics::BodyType::Static;

        RecreateActor(e, type);
    }

    void PhysicsSystem::RecreateActor(Entity e, physics::BodyType type) {
        if (!e.HasComponent<ColliderComponent>()) return;
        auto& cc = e.GetComponentUnchecked<ColliderComponent>();
        const auto& tc = e.GetComponentUnchecked<TransformComponent>();

        // save shapes
        PxShape* shape = cc.shapeHandle;

        // remove old
        m_PhysX->GetScene().removeActor(*cc.actor);
        cc.actor->release();

        // create new
        if (e.HasComponent<PhysicsBodyComponent>()) {
            const auto& pb = e.GetComponentUnchecked<PhysicsBodyComponent>();
            if (pb.bodyType == physics::BodyType::Dynamic) {
                cc.actor = CreateDynamicActor(tc);
            }
            if (pb.bodyType == physics::BodyType::Kinematic) {
                cc.actor = CreateKinematicActor(tc);
            }
        } else {
            cc.actor = CreateStaticActor(tc);
        }

        cc.actor->attachShape(*shape);

        FinalizeActor(e);
    }

    void PhysicsSystem::FinalizeActor(const Entity e) {
        const auto& cc = e.GetComponentUnchecked<ColliderComponent>();

        if (e.HasComponent<PhysicsBodyComponent>()) {
            const auto& pb = e.GetComponentUnchecked<PhysicsBodyComponent>();

            if (pb.bodyType == physics::BodyType::Dynamic) {
                auto* body = cc.actor->is<PxRigidDynamic>();
                // MassAndInertia are computed from attached shapes,
                // Therefore, we should call updateMassAndInertia after attaching the shapes.
                PxRigidBodyExt::updateMassAndInertia(*body, pb.mass);
                body->setAngularDamping(0.5f);
            }

            if (pb.bodyType == physics::BodyType::Kinematic) {
                auto* body = cc.actor->is<PxRigidDynamic>();
                body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
            }
        }

        m_PhysX->GetScene().addActor(*cc.actor);
    }

    PxRigidStatic* PhysicsSystem::CreateStaticActor(const TransformComponent &tc) {
        auto* sActor = m_PhysX->GetPhysics().createRigidStatic(util::RealToPX(tc.transform));
        return sActor;
    }

    PxRigidDynamic* PhysicsSystem::CreateDynamicActor(const TransformComponent &tc) {
        auto* dynamic = m_PhysX->GetPhysics().createRigidDynamic(util::RealToPX(tc.transform));
        // set object's rotation speed (angular damping)
        dynamic->setAngularDamping(0.5f);
        return dynamic;
    }

    PxRigidDynamic* PhysicsSystem::CreateKinematicActor(const TransformComponent &tc) {
        auto* kinematicBody = m_PhysX->GetPhysics().createRigidDynamic(util::RealToPX(tc.transform));
        kinematicBody->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
        return kinematicBody;
    }
}
