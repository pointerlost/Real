//
// Created by pointerlost on 1/13/26.
//
#pragma once
#include "ISystem.h"
#include "Core/Utils.h"
#include "Scene/ISceneListener.h"

namespace Real {
    class Entity;

    namespace physics {
        class IPhysicsBackend;
    }
}

namespace Real::ecs {

    class PhysicsSystem final : public ISystem, public ISceneListener {
    public:
        explicit PhysicsSystem(Scope<physics::IPhysicsBackend> backend);
        void Init() override;
        void Update(Scene *scene, f32 deltaTime) override;
        void Shutdown() override;

        void OnSceneAttach(Scene *scene) override;
        void OnSceneDetach(Scene *scene) override;

    private:
        void OnColliderAdded(const Entity& e);
        void OnColliderRemoved(Entity& e);
        void OnColliderChanged(const Entity& e);
        void OnPhysicsBodyAdded(const Entity& e);
        void OnPhysicsBodyChanged(const Entity& e);

        void RebuildCollider(const Entity &e);

        void SubmitColliderDebug(const Entity& e);

        void RegisterEventCallbacks(Scene* scene);

        void SyncTransform(Entity& entity);
        void SyncCollider(Entity& entity);

    private:
        Scope<physics::IPhysicsBackend> m_Backend;
    };

}
