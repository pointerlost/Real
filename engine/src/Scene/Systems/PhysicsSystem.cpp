//
// Created by pointerlost on 1/14/26.
//
#include <Scene/Systems/PhysicsSystem.h>
#include "Core/Services.h"
#include "Physics/Physx/PhysXBackend.h"
#include "Graphics/Debug/DebugRenderer.h"
#include "Physics/PhysicsDescriptors.h"
#include "Scene/Components.h"
#include "Scene/Scene.h"

namespace Real::ecs {

    PhysicsSystem::PhysicsSystem(Scope<physics::IPhysicsBackend> backend) : m_Backend(std::move(backend))
    {
    }

    void PhysicsSystem::Init() {
        // Configure physics world parameters
        physics::PhysicsWorldDesc desc;
        desc.gravity = { 0.f, -9.81f, 0.f };

        // Initialize backend with world configuration
        m_Backend->Init(desc);
    }

    void PhysicsSystem::Update(entt::registry& registry, f32 deltaTime) {
        if (deltaTime <= 0.0f)
            return;

        // Advance physics simulation
        m_Backend->Step(deltaTime);

        auto view = registry.view<TransformComponent, RigidbodyComponent>();

        // Synchronize physics state with ECS
        for (auto entity : view) {
            SyncTransform(entity);
            SyncCollider(entity);
            SubmitColliderDebug(entity);
        }

    }

    void PhysicsSystem::Shutdown() {
        // Shutdown physics backend
        m_Backend->Shutdown();
    }

    void PhysicsSystem::OnColliderAdded(const entt::entity &e) {
        // Collider requires rigid body
        if (!m_Registry->any_of<RigidbodyComponent>(e))
            return;

        auto& rb = m_Registry->get<RigidbodyComponent>(e);
        auto& cc = m_Registry->get<ColliderComponent>(e);

        physics::ShapeDesc sd;
        sd.shape = cc.shape;
        sd.isTrigger = cc.isTrigger;

        cc.handle = m_Backend->CreateShape(sd);

        // Attach shape if body already exists
        if (rb.handle != physics::InvalidRigidBodyHandle)
            m_Backend->AttachShape(rb.handle, cc.handle);
    }

    void PhysicsSystem::OnColliderRemoved(entt::entity &e) {
        if (!m_Registry->any_of<ColliderComponent>(e))
            return;

        auto& cc = m_Registry->get<ColliderComponent>(e);

        if (cc.handle != physics::InvalidShapeHandle) {
            m_Backend->DestroyShape(cc.handle);
            cc.handle = physics::InvalidShapeHandle;
        }
    }

    void PhysicsSystem::OnColliderChanged(const entt::entity &e) {
        // Require both body and collider
        if (!m_Registry->any_of<RigidbodyComponent>(e) || !m_Registry->any_of<ColliderComponent>(e))
            return;

        auto& rb = m_Registry->get<RigidbodyComponent>(e);
        auto& cc = m_Registry->get<ColliderComponent>(e);

        if (rb.handle == physics::InvalidRigidBodyHandle)
            return;

        if (cc.rebuildRequired) {
            RebuildCollider(e);
            cc.rebuildRequired = false;
        }
    }

    void PhysicsSystem::OnPhysicsBodyAdded(const entt::entity &e) {
        if (!m_Registry->any_of<RigidbodyComponent>(e))
            return;

        auto& rb = m_Registry->get<RigidbodyComponent>(e);
        auto& tc = m_Registry->get<TransformComponent>(e);

        physics::BodyDesc bd;
        bd.type = rb.type;
        bd.worldTransform = tc.transform;
        bd.mass = rb.mass;

        rb.handle = m_Backend->CreateBody(bd);

        // If collider exists, create and attach shape
        if (!m_Registry->any_of<ColliderComponent>(e)) {
            auto& cc = m_Registry->get<ColliderComponent>(e);

            physics::ShapeDesc sd;
            sd.shape = cc.shape;
            sd.isTrigger = cc.isTrigger;

            cc.handle = m_Backend->CreateShape(sd);

            m_Backend->AttachShape(rb.handle, cc.handle);
        }
    }

    void PhysicsSystem::OnPhysicsBodyChanged(const entt::entity &e) {
        if (!m_Registry->any_of<RigidbodyComponent>(e))
            return;

        auto& rb = m_Registry->get<RigidbodyComponent>(e);

        // Destroy old body if exists
        if (rb.handle != physics::InvalidRigidBodyHandle)
            m_Backend->DestroyBody(rb.handle);

        // Recreate body with new settings
        OnPhysicsBodyAdded(e);
    }

    // TODO: Remove this function (BROKEN SRP, GLOBAL STATE)
    void PhysicsSystem::SubmitColliderDebug(const entt::entity& e) {
        if (!m_Registry->any_of<ColliderComponent>(e))
            return;

        auto& cc = m_Registry->get<ColliderComponent>(e);
        auto& tc = m_Registry->get<TransformComponent>(e);

        auto* debugRenderer = Services::GetDebugRenderer();
        const auto& modelMatrix = tc.transform.GetModelMatrix();

        if (cc.shape == physics::ColliderShape::Box) {
            debugRenderer->DrawBox(modelMatrix, math::Vec4(1.0, 0.0, 0.0, 1.0));
        }
        if (cc.shape == physics::ColliderShape::Capsule) {
            debugRenderer->DrawCapsule(math::Vec3(0, 0, 0), math::Vec3(0, 5, 0), 0.5f, math::Vec4(1, 0, 0, 1));
        }
        if (cc.shape == physics::ColliderShape::Sphere) {
            debugRenderer->DrawSphere(modelMatrix, math::Vec4(0.0, 1.0, 0.0, 1.0));
        }

        // TODO:
        // if (cc.debug.show) {
        // }
    }

    void PhysicsSystem::OnSceneAttach(entt::registry& registry, event::SceneEvents& events) {
        m_Registry = &registry;
        // Register physics-related ECS event callbacks
        RegisterEventCallbacks(events);
    }

    void PhysicsSystem::OnSceneDetach(entt::registry& registry, event::SceneEvents& events) {
    }

    void PhysicsSystem::RegisterEventCallbacks(event::SceneEvents& events) {
        events.OnColliderAdded.Subscribe(
            [this](const entt::entity& e, ColliderComponent&) {
                OnColliderAdded(e);
            }
        );

        events.OnColliderChanged.Subscribe(
            [this](const entt::entity& e, physics::ColliderChangeType type) {
                switch (type) {
                    case physics::ColliderChangeType::Dirty:
                        // TODO: Update collider stuff
                        break;

                    case physics::ColliderChangeType::Rebuild:
                        RebuildCollider(e);
                        break;
                    default: ;
                }
            }
        );

        events.OnPhysicsBodyAdded.Subscribe(
            [this](const entt::entity& e, RigidbodyComponent&) {
                OnPhysicsBodyAdded(e);
            }
        );

        events.OnPhysicsBodyChanged.Subscribe(
            [this](const entt::entity& e, RigidbodyComponent&) {
                OnPhysicsBodyChanged(e);
            }
        );
    }

    void PhysicsSystem::RebuildCollider(const entt::entity& e) {
        if (!m_Registry->any_of<RigidbodyComponent>(e))
            return;

        auto& rb = m_Registry->get<RigidbodyComponent>(e);
        auto& cc = m_Registry->get<ColliderComponent>(e);

        if (cc.handle != physics::InvalidShapeHandle) {
            m_Backend->DetachShape(rb.handle, cc.handle);
            m_Backend->DestroyShape(cc.handle);
        }

        physics::ShapeDesc sd;
        sd.shape = cc.shape;
        sd.isTrigger = cc.isTrigger;

        cc.handle = m_Backend->CreateShape(sd);
        m_Backend->AttachShape(rb.handle, cc.handle);
    }

    void PhysicsSystem::SyncTransform(const entt::entity& entity) {
        if (!m_Registry->any_of<RigidbodyComponent>(entity))
            return;

        auto& rb = m_Registry->get<RigidbodyComponent>(entity);
        auto& tc = m_Registry->get<TransformComponent>(entity);

        if (rb.handle == physics::InvalidRigidBodyHandle)
            return;

        // Dynamic bodies update transform from physics
        if (rb.type == physics::BodyType::Dynamic)
            tc.transform = m_Backend->GetBodyTransform(rb.handle);
        else
            // Static and kinematic bodies push transform to physics
            m_Backend->SetBodyTransform(rb.handle, tc.transform);
    }

    void PhysicsSystem::SyncCollider(entt::entity& entity) {
        if (!m_Registry->any_of<RigidbodyComponent>(entity) || !m_Registry->any_of<ColliderComponent>(entity))
            return;

        auto& rb = m_Registry->get<RigidbodyComponent>(entity);
        auto& cc = m_Registry->get<ColliderComponent>(entity);

        if (rb.handle == physics::InvalidRigidBodyHandle || cc.handle == physics::InvalidShapeHandle)
            return;

        m_Backend->SetShapeLocalTransform(cc.handle, cc.localPosition, cc.localRotation);

        // Update collider enabled state
        m_Backend->SetShapeEnabled(cc.handle, cc.enabled);

        // Rebuild only if explicitly requested
        if (cc.rebuildRequired) {
            RebuildCollider(entity);
            cc.rebuildRequired = false;
        }
    }
}
