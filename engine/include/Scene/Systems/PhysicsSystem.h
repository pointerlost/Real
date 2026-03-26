//
// Created by pointerlost on 1/13/26.
//
#pragma once
#include "Core/ISystem.h"
#include "Scene/ISceneListener.h"

namespace Real {
    namespace core {
        class IPhysicsBackend;
    }

    namespace event {
        class SceneEvents;
    }
}

namespace Real::ecs {

    class PhysicsSystem final : public ISystem, public ISceneListener {
    public:
        explicit PhysicsSystem(core::IPhysicsBackend& backend);
        void Init() override;
        void Update(entt::registry& registry, f32 deltaTime) override;
        void Shutdown() override;

        void OnSceneAttach(entt::registry& registry, event::SceneEvents& events) override;
        void OnSceneDetach(entt::registry& registry, event::SceneEvents& events) override;

    private:
        void OnColliderAdded(const entt::entity& e)      const;
        void OnColliderRemoved(entt::entity& e)          const;
        void OnColliderChanged(const entt::entity& e)    const;
        void OnPhysicsBodyAdded(const entt::entity& e)   const;
        void OnPhysicsBodyChanged(const entt::entity& e) const;

        void RebuildCollider(const entt::entity &e) const;

        void SubmitColliderDebug(const entt::entity& e) const;

        void RegisterEventCallbacks(event::SceneEvents& events);

        void SyncTransform(const entt::entity& entity) const;
        void SyncCollider(entt::entity& entity);

    private:
        core::IPhysicsBackend& m_Backend;
        entt::registry*        m_Registry = nullptr;
    };

}
