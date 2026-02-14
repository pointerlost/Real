//
// Created by pointerlost on 1/14/26.
//
#include <Scene/Systems/LightSystem.h>
#include "Scene/Components.h"
#include "Scene/Scene.h"

namespace Real::ecs {

    void LightSystem::Init() {
    }

    void LightSystem::Update(Scene *scene, f32 deltaTime) {
        auto view = scene->GetRegistry().view<LightComponent, TransformComponent>();

        for (const auto entity : view) {
            auto& light     = view.get<LightComponent>(entity);
            auto& transform = view.get<TransformComponent>(entity);

            light.m_Light.Update(transform.transform);
        }
    }

    void LightSystem::Shutdown() {
    }
}
