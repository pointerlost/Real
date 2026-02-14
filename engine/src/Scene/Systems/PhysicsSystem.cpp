//
// Created by pointerlost on 1/14/26.
//
#include <Scene/Systems/PhysicsSystem.h>
#include "Core/Services.h"
#include "Physics/Physx/PhysXBackend.h"
#include "Graphics/Debug/DebugRenderer.h"
#include "Physics/PhysicsDescriptors.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"
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

    void PhysicsSystem::Update(Scene *scene, f32 deltaTime) {
        if (deltaTime <= 0.0f)
            return;

        // Advance physics simulation
        m_Backend->Step(deltaTime);

        auto& entities = scene->GetEntities();

        // Synchronize physics state with ECS
        for (auto& entity : std::views::values(entities)) {
            SyncTransform(entity);
            SyncCollider(entity);
            SubmitColliderDebug(entity);
        }
    }

    void PhysicsSystem::Shutdown() {
        // Shutdown physics backend
        m_Backend->Shutdown();
    }

    void PhysicsSystem::OnColliderAdded(const Entity &e) {
        // Collider requires rigid body
        if (!e.HasComponent<RigidBodyComponent>())
            return;

        auto& rb = e.GetComponentUnchecked<RigidBodyComponent>();
        auto& cc = e.GetComponentUnchecked<ColliderComponent>();

        physics::ShapeDesc sd;
        sd.shape = cc.shape;
        sd.isTrigger = cc.isTrigger;

        cc.handle = m_Backend->CreateShape(sd);

        // Attach shape if body already exists
        if (rb.handle != physics::InvalidRigidBodyHandle)
            m_Backend->AttachShape(rb.handle, cc.handle);
    }

    void PhysicsSystem::OnColliderRemoved(Entity &e) {
        if (!e.HasComponent<ColliderComponent>())
            return;

        auto& col = e.GetComponentUnchecked<ColliderComponent>();

        if (col.handle != physics::InvalidShapeHandle) {
            m_Backend->DestroyShape(col.handle);
            col.handle = physics::InvalidShapeHandle;
        }
    }

    void PhysicsSystem::OnColliderChanged(const Entity &e) {
        // Require both body and collider
        if (!e.HasComponent<RigidBodyComponent>() || !e.HasComponent<ColliderComponent>())
            return;

        auto& rb = e.GetComponentUnchecked<RigidBodyComponent>();
        auto& cc = e.GetComponentUnchecked<ColliderComponent>();

        if (rb.handle == physics::InvalidRigidBodyHandle)
            return;

        if (cc.rebuildRequired) {
            RebuildCollider(e);
            cc.rebuildRequired = false;
        }
    }

    void PhysicsSystem::OnPhysicsBodyAdded(const Entity &e) {
        auto& rb = e.GetComponentUnchecked<RigidBodyComponent>();
        auto& tc = e.GetComponentUnchecked<TransformComponent>();

        physics::BodyDesc bd;
        bd.type = rb.type;
        bd.worldTransform = tc.transform;
        bd.mass = rb.mass;

        rb.handle = m_Backend->CreateBody(bd);

        // If collider exists, create and attach shape
        if (e.HasComponent<ColliderComponent>()) {
            auto& cc = e.GetComponentUnchecked<ColliderComponent>();

            physics::ShapeDesc sd;
            sd.shape = cc.shape;
            sd.isTrigger = cc.isTrigger;

            cc.handle = m_Backend->CreateShape(sd);

            m_Backend->AttachShape(rb.handle, cc.handle);
        }
    }

    void PhysicsSystem::OnPhysicsBodyChanged(const Entity &e) {
        auto& rb = e.GetComponentUnchecked<RigidBodyComponent>();

        // Destroy old body if exists
        if (rb.handle != physics::InvalidRigidBodyHandle)
            m_Backend->DestroyBody(rb.handle);

        // Recreate body with new settings
        OnPhysicsBodyAdded(e);
    }

    void PhysicsSystem::SubmitColliderDebug(const Entity& e) {
        if (!e.HasComponent<ColliderComponent>())
            return;

        const auto& cc = e.GetComponentUnchecked<ColliderComponent>();
        const auto& tc = e.GetComponentUnchecked<TransformComponent>();

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

        // if (cc.debug.show) {
        // }
    }

    void PhysicsSystem::OnSceneAttach(Scene *scene) {
        // Register physics-related ECS event callbacks
        RegisterEventCallbacks(scene);
    }

    void PhysicsSystem::OnSceneDetach(Scene *scene) {
    }

    void PhysicsSystem::RegisterEventCallbacks(Scene *scene) {
        auto& events = scene->GetEvents();

        events.OnColliderAdded.Subscribe(
            [this](const Entity e, ColliderComponent&) {
                OnColliderAdded(e);
            }
        );

        events.OnColliderChanged.Subscribe(
            [this](const Entity& e, physics::ColliderChangeType type) {
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
            [this](const Entity e, RigidBodyComponent&) {
                OnPhysicsBodyAdded(e);
            }
        );

        events.OnPhysicsBodyChanged.Subscribe(
            [this](const Entity e, RigidBodyComponent&) {
                OnPhysicsBodyChanged(e);
            }
        );
    }

    void PhysicsSystem::RebuildCollider(const Entity &e) {
        if (!e.HasComponent<RigidBodyComponent>())
            return;

        auto& rb = e.GetComponentUnchecked<RigidBodyComponent>();
        auto& cc = e.GetComponentUnchecked<ColliderComponent>();

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

    void PhysicsSystem::SyncTransform(Entity &entity) {
        if (!entity.HasComponent<RigidBodyComponent>())
            return;

        auto& rb = entity.GetComponentUnchecked<RigidBodyComponent>();
        auto& tc = entity.GetComponentUnchecked<TransformComponent>();

        if (rb.handle == physics::InvalidRigidBodyHandle)
            return;

        // Dynamic bodies update transform from physics
        if (rb.type == physics::BodyType::Dynamic)
            tc.transform = m_Backend->GetBodyTransform(rb.handle);
        else
            // Static and kinematic bodies push transform to physics
            m_Backend->SetBodyTransform(rb.handle, tc.transform);
    }

    void PhysicsSystem::SyncCollider(Entity &entity) {
        if (!entity.HasComponent<RigidBodyComponent>() || !entity.HasComponent<ColliderComponent>())
            return;

        auto& rb = entity.GetComponentUnchecked<RigidBodyComponent>();
        auto& cc = entity.GetComponentUnchecked<ColliderComponent>();

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
