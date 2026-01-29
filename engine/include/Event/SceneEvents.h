//
// Created by pointerlost on 1/26/26.
//
#pragma once
#include "Signal.h"

namespace Real {
    struct PhysicsBodyComponent;
    struct ModelComponent;
    class Entity;
}

namespace Real::event {

    class SceneEvents {
    public:
        Event<Entity&, ModelComponent&> OnModelAssigned;
        Event<Entity&, PhysicsBodyComponent&> OnPhysicsBodyAdded;
    };
}
