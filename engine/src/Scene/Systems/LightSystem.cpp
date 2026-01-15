//
// Created by pointerlost on 1/14/26.
//
#include <Scene/Systems/LightSystem.h>
#include "Scene/Components.h"
#include "Scene/Scene.h"

namespace Real::ecs {

    void LightSystem::Init() {
    }

    void LightSystem::Update(Scene *scene, float deltaTime) {
        const auto& view = scene->GetAllEntitiesWith<LightComponent, TransformComponent>();

        for (const auto& [entity, light, transform] : view.each()) {
            light.m_Light.Update(transform.transform);
        }
    }

    void LightSystem::Shutdown() {
    }
}
