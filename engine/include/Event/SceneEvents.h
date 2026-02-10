//
// Created by pointerlost on 1/26/26.
//
#pragma once
#include "Event.h"
#include "Physics/PhysicsTypes.h"

namespace Real {
    struct PhysicsBodyComponent;
    struct ColliderComponent;
    struct ModelComponent;
    class Entity;
}

namespace Real::event {

    class SceneEvents {
    public:
        Event<Entity&, ModelComponent&> OnModelAssigned;
        Event<Entity&, ColliderComponent&> OnColliderAdded;
        Event<Entity&, ColliderComponent&, physics::ColliderChangeType> OnColliderChanged;
        Event<Entity&, PhysicsBodyComponent&> OnPhysicsBodyAdded;
        Event<Entity&, PhysicsBodyComponent&> OnPhysicsBodyChanged;
    };
}
