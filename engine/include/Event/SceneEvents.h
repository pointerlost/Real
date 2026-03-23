//
// Created by pointerlost on 1/26/26.
//
#pragma once
#include "Event.h"

namespace entt {
    enum class entity : std::uint32_t;
}

namespace Real {
    namespace physics {
        enum class ColliderChangeType;
    }

    struct RigidbodyComponent;
    struct ColliderComponent;
    struct ModelComponent;
}

namespace Real::event {

    class SceneEvents {
    public:
        Event<entt::entity&, ModelComponent&> OnModelAssigned;
        Event<entt::entity&, ColliderComponent&> OnColliderAdded;
        Event<entt::entity&, physics::ColliderChangeType> OnColliderChanged;
        Event<entt::entity&, RigidbodyComponent&> OnPhysicsBodyAdded;
        Event<entt::entity&, RigidbodyComponent&> OnPhysicsBodyChanged;
    };
}
