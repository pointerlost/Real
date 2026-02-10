//
// Created by pointerlost on 1/14/26.
//
#include <Scene/Systems/PhysicsSystem.h>
#include "Core/Logger.h"
#include "Core/Services.h"
#include "Graphics/Debug/DebugRenderer.h"
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
            auto& pb = view.get<PhysicsBodyComponent>(entity);
            auto& cc = view.get<ColliderComponent>(entity);

            SubmitColliderDebug(tc, cc);

            if (pb.bodyType != physics::BodyType::Dynamic)
                continue;

            const auto* actor = cc.actor->is<PxRigidDynamic>();
            if (!actor) continue;

            // Set position and rotation via PhysX but PhysX doesn't provide to us Scaling!!!
            const PxTransform pxT = actor->getGlobalPose();
            tc.transform.position = util::PXToReal(pxT.p);
            tc.transform.rotation = util::PXToReal(pxT.q);
        }
    }

    void PhysicsSystem::Shutdown() {
        m_PhysX->Shutdown(); // PhysX lifetime and low-level states like Physics, Foundation(PhysX) etc.
    }

    void PhysicsSystem::SetRegistry(entt::registry &registry) {
        m_Registry = &registry;
    }

    void PhysicsSystem::OnSceneAttach(Scene *scene) {
        RegisterEventCallbacks(scene);
    }

    void PhysicsSystem::OnSceneDetach(Scene *scene) {
    }

    void PhysicsSystem::OnPhysicsBodyAdded(Entity e, Scene *scene) {
        SyncPhysics(e);
    }

    void PhysicsSystem::OnPhysicsBodyChanged(Entity e, Scene *scene) {
        if (!e.HasComponent<ColliderComponent>())
            return;

        // Changing body type requires full actor recreation
        SyncPhysics(e);
    }

    void PhysicsSystem::OnColliderAdded(Entity e, Scene *scene) {
        auto& cc = e.GetComponentUnchecked<ColliderComponent>();

        if (!cc.actor) {
            CreateBody(e);
        }

        CreateColliderShape(cc);

        if (cc.enabled) {
            cc.actor->attachShape(*cc.shapeHandle);
            cc.attached = true;
        }

        FinalizeActor(e);
    }

    void PhysicsSystem::OnColliderChanged(Entity e, Scene* scene) {
        if (!e.HasComponent<ColliderComponent>())
            return;

        const auto& cc = e.GetComponentUnchecked<ColliderComponent>();

        // Enable / Disable toggle
        if (cc.enabled && !cc.attached) {
            EnableCollider(e);
        }
        else if (!cc.enabled && cc.attached) {
            DisableCollider(e);
            return;
        }

        // Sync pose only (safe)
        if (cc.shapeHandle && cc.attached) {
            cc.shapeHandle->setLocalPose(
                PxTransform(
                    util::RealToPX(cc.localPosition),
                    util::RealToPX(cc.localRotation)
                )
            );
        }
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

        m_PhysX->GetScene().removeActor(*cc.actor);
        cc.actor->release();
        cc.actor = nullptr;
        cc.attached = false;

        if (cc.shapeHandle) {
            cc.shapeHandle->release();
            cc.shapeHandle = nullptr;
        }

        cc.actor = CreateBody(e);
        CreateColliderShape(cc);

        if (cc.enabled) {
            cc.actor->attachShape(*cc.shapeHandle);
            cc.attached = true;
        }

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

    void PhysicsSystem::SubmitColliderDebug(const TransformComponent& tc, ColliderComponent& cc) {
        if (cc.debug.show) {
            auto* debugRenderer = Services::GetDebugRenderer();
            const auto& modelMatrix = tc.transform.GetModelMatrix();

            if (cc.shape == physics::ColliderShape::Box) {
                debugRenderer->DrawBox(modelMatrix, math::Vec4(1.0, 0.0, 0.0, 1.0));
            }
            if (cc.shape == physics::ColliderShape::Capsule) {
            }
            if (cc.shape == physics::ColliderShape::Sphere) {
                debugRenderer->DrawSphere(modelMatrix, math::Vec4(0.0, 1.0, 0.0, 1.0));
            }
        }
    }

    void PhysicsSystem::EnableCollider(Entity e) {
        auto& cc = e.GetComponentUnchecked<ColliderComponent>();
        if (!cc.actor || !cc.shapeHandle || cc.attached)
            return;

        cc.actor->attachShape(*cc.shapeHandle);
        cc.attached = true;
    }

    void PhysicsSystem::DisableCollider(Entity e) {
        auto& cc = e.GetComponentUnchecked<ColliderComponent>();
        if (!cc.actor || !cc.shapeHandle || !cc.attached)
            return;

        cc.actor->detachShape(*cc.shapeHandle);
        cc.attached = false;
    }

    void PhysicsSystem::RebuildCollider(Entity e, ColliderComponent& cc) {
        if (!cc.actor)
            return;

        if (cc.attached && cc.shapeHandle) {
            cc.actor->detachShape(*cc.shapeHandle);
            cc.attached = false;
        }

        if (cc.shapeHandle) {
            cc.shapeHandle->release();
            cc.shapeHandle = nullptr;
        }

        CreateColliderShape(cc);

        if (cc.enabled) {
            cc.actor->attachShape(*cc.shapeHandle);
            cc.attached = true;
        }
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

    void PhysicsSystem::RegisterEventCallbacks(Scene* scene) {
        auto& events = scene->GetEvents();

        events.OnColliderAdded.Subscribe(
            [this, scene](const Entity e, ColliderComponent&) {
                OnColliderAdded(e, scene);
            }
        );

        events.OnColliderChanged.Subscribe(
            [this](const Entity& e, ColliderComponent& cc, physics::ColliderChangeType type) {
                switch (type) {
                    case physics::ColliderChangeType::Dirty:
                        // TODO: Update collider stuff
                        break;

                    case physics::ColliderChangeType::Rebuild:
                        RebuildCollider(e, cc);
                        break;
                    default: ;
                }
            }
        );

        events.OnPhysicsBodyAdded.Subscribe(
            [this, scene](const Entity e, PhysicsBodyComponent&) {
                OnPhysicsBodyAdded(e, scene);
            }
        );

        events.OnPhysicsBodyChanged.Subscribe(
            [this, scene](const Entity e, PhysicsBodyComponent&) {
                OnPhysicsBodyChanged(e, scene);
            }
        );
    }

}
