//
// Created by pointerlost on 1/14/26.
//
#include <Scene/Systems/TransformSystem.h>
#include "Scene/Components.h"
#include "Scene/Scene.h"

namespace Real::ecs {

    void TransformSystem::Init() {
    }

    void TransformSystem::Update(Scene *scene, float deltaTime) {
        const auto& view = scene->GetAllEntitiesWith<TransformComponent>();

        for (const auto& [entity, transform] : view.each()) {
            // TODO: need an update helper?
            // transform.transform.Update();
        }
    }

    void TransformSystem::Shutdown() {
    }
}
