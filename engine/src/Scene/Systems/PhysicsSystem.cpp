//
// Created by pointerlost on 1/14/26.
//
#include <Scene/Systems/PhysicsSystem.h>
#include "Core/IPhysicsBackend.h"
#include "Core/Services.h"
#include "Graphics/Debug/DebugRenderer.h"
#include "Scene/Components.h"
#include "Scene/Scene.h"

namespace Real::ecs {

    PhysicsSystem::PhysicsSystem(core::IPhysicsBackend& backend)
        : m_Backend(backend)
    {
    }

    void PhysicsSystem::Init() {
        // Configure physics world parameters
        core::PhysicsWorldDesc desc;
        desc.gravity = { 0.f, -9.81f, 0.f };

        // Initialize backend with world configuration
        m_Backend.Init(desc);
    }

    void PhysicsSystem::Update(entt::registry& registry, f32 deltaTime) {
        if (deltaTime <= 0.0f)
            return;

        // Advance physics simulation
        m_Backend.Step(deltaTime);

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
        m_Backend.Shutdown();
    }

    void PhysicsSystem::OnColliderAdded(const entt::entity &e) const {
        // Collider requires rigid body
        if (!m_Registry->any_of<RigidbodyComponent>(e))
            return;

        auto& [rbDesc, rbHandle] = m_Registry->get<RigidbodyComponent>(e);
        auto& [ccDesc, ccHandle] = m_Registry->get<ColliderComponent>(e);

        core::ShapeDesc sd;
        sd.shape     = ccDesc.shape;
        sd.isTrigger = ccDesc.isTrigger;

        ccHandle = m_Backend.CreateShape(sd);

        // Attach shape if body already exists
        if (rbHandle.id != 0)
            m_Backend.AttachShape(rbHandle, ccHandle);
    }

    void PhysicsSystem::OnColliderRemoved(entt::entity &e) const {
        if (!m_Registry->any_of<ColliderComponent>(e))
            return;

        if (auto&[desc, handle] = m_Registry->get<ColliderComponent>(e); handle.id != 0) {
            m_Backend.DestroyShape(handle);
            handle = core::ShapeHandle{};
        }
    }

    void PhysicsSystem::OnColliderChanged(const entt::entity &e) const {
        // Require both body and collider
        if (!m_Registry->any_of<RigidbodyComponent>(e) || !m_Registry->any_of<ColliderComponent>(e))
            return;

        auto& rb = m_Registry->get<RigidbodyComponent>(e);
        auto& cc = m_Registry->get<ColliderComponent>(e);

        if (rb.handle.id == 0)
            return;

        if (cc.desc.rebuildRequired) {
            RebuildCollider(e);
            cc.desc.rebuildRequired = false;
        }
    }

    void PhysicsSystem::OnPhysicsBodyAdded(const entt::entity &e) const {
        if (!m_Registry->any_of<RigidbodyComponent>(e))
            return;

        auto&[rbDesc, rbHandle] = m_Registry->get<RigidbodyComponent>(e);
        const auto& tc = m_Registry->get<TransformComponent>(e);

        core::BodyDesc bd;
        bd.type           = rbDesc.type;
        bd.localTransform = core::LocalPose{ tc.transform.GetLocalPosition(), tc.transform.GetLocalRotation() };
        bd.mass           = rbDesc.mass;

        rbHandle = m_Backend.CreateBody(bd);

        // If collider exists, create and attach shape
        if (!m_Registry->any_of<ColliderComponent>(e)) {
            auto& [ccDesc, ccHandle] = m_Registry->get<ColliderComponent>(e);

            core::ShapeDesc sd;
            sd.shape     = ccDesc.shape;
            sd.isTrigger = ccDesc.isTrigger;

            ccHandle = m_Backend.CreateShape(sd);

            m_Backend.AttachShape(rbHandle, ccHandle);
        }
    }

    void PhysicsSystem::OnPhysicsBodyChanged(const entt::entity &e) const {
        if (!m_Registry->any_of<RigidbodyComponent>(e))
            return;

        auto& [desc, handle] = m_Registry->get<RigidbodyComponent>(e);

        // Destroy old body if exists
        if (handle.id != 0)
            m_Backend.DestroyBody(handle);

        // Recreate body with new settings
        OnPhysicsBodyAdded(e);
    }

    // TODO: Remove this function (BROKEN SRP, GLOBAL STATE)
    void PhysicsSystem::SubmitColliderDebug(const entt::entity& e) const {
        if (!m_Registry->any_of<ColliderComponent>(e))
            return;

        auto&[desc, handle] = m_Registry->get<ColliderComponent>(e);
        const auto& tc = m_Registry->get<TransformComponent>(e);

        auto* debugRenderer     = Services::GetDebugRenderer();
        const auto& modelMatrix = tc.transform.GetWorldMatrix();

        if (desc.shape == core::ShapeDesc::Shape::Box) {
            debugRenderer->DrawBox(modelMatrix, math::Vec4(1.0, 0.0, 0.0, 1.0));
        }
        if (desc.shape == core::ShapeDesc::Shape::Capsule) {
            debugRenderer->DrawCapsule(math::Vec3(0, 0, 0), math::Vec3(0, 5, 0), 0.5f, math::Vec4(1, 0, 0, 1));
        }
        if (desc.shape == core::ShapeDesc::Shape::Sphere) {
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

    void PhysicsSystem::RebuildCollider(const entt::entity& e) const {
        if (!m_Registry->any_of<RigidbodyComponent>(e))
            return;

        auto& rb = m_Registry->get<RigidbodyComponent>(e);
        auto& cc = m_Registry->get<ColliderComponent>(e);

        if (cc.handle.id != 0) {
            m_Backend.DetachShape(rb.handle, cc.handle);
            m_Backend.DestroyShape(cc.handle);
        }

        core::ShapeDesc sd;
        sd.shape     = cc.desc.shape;
        sd.isTrigger = cc.desc.isTrigger;

        cc.handle = m_Backend.CreateShape(sd);
        m_Backend.AttachShape(rb.handle, cc.handle);
    }

    void PhysicsSystem::SyncTransform(const entt::entity& entity) const {
        if (!m_Registry->any_of<RigidbodyComponent>(entity))
            return;

        auto& rb = m_Registry->get<RigidbodyComponent>(entity);
        auto& tc = m_Registry->get<TransformComponent>(entity);

        if (rb.handle.id == 0)
            return;

        // Dynamic bodies update transform from physics
        if (rb.desc.type == core::BodyDesc::Type::Dynamic) {
            auto [pos,rot] = m_Backend.GetBodyTransform(rb.handle);
            tc.transform.SetLocalPosition(pos);
            tc.transform.SetLocalRotation(rot);
        }
        else {
            // Static and kinematic bodies push transform to physics
            m_Backend.SetBodyTransform(rb.handle, { tc.transform.GetLocalPosition(), tc.transform.GetLocalRotation() });
        }
    }

    void PhysicsSystem::SyncCollider(entt::entity& entity) {
        if (!m_Registry->any_of<RigidbodyComponent>(entity) || !m_Registry->any_of<ColliderComponent>(entity))
            return;

        auto& rb = m_Registry->get<RigidbodyComponent>(entity);
        auto& cc = m_Registry->get<ColliderComponent>(entity);

        if (rb.handle.id == 0 || cc.handle.id == 0)
            return;

        m_Backend.SetBodyTransform(rb.handle, cc.desc.localTransform);

        // Update collider enabled state
        m_Backend.AttachShape(rb.handle, cc.handle);

        // Rebuild only if explicitly requested
        if (cc.desc.rebuildRequired) {
            RebuildCollider(entity);
            cc.desc.rebuildRequired = false;
        }
    }
}
