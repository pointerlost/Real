//
// Created by pointerlost on 1/13/26.
//
#pragma once
#include "Systems.h"
#include "Physics/PhysXContext.h"
#include "Core/Utils.h"
#include "entt/entt.hpp"
#include "Scene/Components.h"

namespace Real {
    struct PhysicsBodyComponent;
}

namespace Real {
    struct TransformComponent;
}

namespace Real {
    struct ColliderComponent;
}

namespace Real {
    class Entity;
}

namespace Real::ecs {

    // This class manages: REAL engine logic and ECS integration (with PhysX)
    class PhysicsSystem final : public Systems {
    public:
        void Init() override;
        void Update(Scene* scene, float deltaTime) override;
        void Shutdown() override;
        void SetRegistry(entt::registry &registry) override;

        void OnSceneAttach(Scene* scene) override;
        void OnSceneDetach(Scene* scene) override;

        void OnPhysicsBodyAdded(Entity e, Scene *scene);
        void OnPhysicsBodyChanged(Entity e, Scene *scene);
        void OnColliderAdded(Entity e, Scene *scene);
        void OnColliderChanged(Entity e, Scene *scene);

    private:
        physx::PxRigidActor* CreateBody(Entity& entity);
        physx::PxShape* CreateColliderShape(ColliderComponent& cc);
        void AttachCollider(physx::PxRigidActor* actor, physx::PxShape* shape);
        void SyncPhysics(Entity e);
        void RecreateActor(Entity e, physics::BodyType type);
        void FinalizeActor(Entity e);

        void SubmitColliderDebug(const TransformComponent& tc, ColliderComponent& cc);
        void EnableCollider(Entity e);
        void DisableCollider(Entity e);
        void RebuildCollider(Entity e, ColliderComponent& cc);

        physx::PxRigidStatic*  CreateStaticActor(const TransformComponent &tc);
        physx::PxRigidDynamic* CreateDynamicActor(const TransformComponent &tc);
        physx::PxRigidDynamic* CreateKinematicActor(const TransformComponent &tc);

        void RegisterEventCallbacks(Scene* scene);

    private:
        Scope<physics::PhysXContext> m_PhysX;
        entt::registry* m_Registry = nullptr;
    };

}
