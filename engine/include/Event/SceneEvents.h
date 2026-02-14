//
// Created by pointerlost on 1/26/26.
//
#pragma once
#include "Event.h"
#include "Physics/PhysicsTypes.h"

namespace Real {
    struct RigidBodyComponent;
    struct ColliderComponent;
    struct ModelComponent;
    class Entity;
}

namespace Real::event {

    class SceneEvents {
    public:
        Event<Entity&, ModelComponent&> OnModelAssigned;
        Event<Entity&, ColliderComponent&> OnColliderAdded;
        Event<Entity&, physics::ColliderChangeType> OnColliderChanged;
        Event<Entity&, RigidBodyComponent&> OnPhysicsBodyAdded;
        Event<Entity&, RigidBodyComponent&> OnPhysicsBodyChanged;
    };
}
