//
// Created by pointerlost on 1/14/26.
//
#include <Scene/Systems/CameraSystem.h>

#include "Core/Logger.h"
#include "Scene/Components.h"
#include "Scene/Scene.h"

namespace Real::ecs {

    void CameraSystem::Init() {
    }

    void CameraSystem::Update(entt::registry& registry, f32 deltaTime) {
        auto view = registry.view<CameraComponent, TransformComponent>();

        for (const auto entity : view) {
            auto& camera    = view.get<CameraComponent>(entity);
            auto& transform = view.get<TransformComponent>(entity);

            camera.m_Camera.Update(transform.transform);
        }
    }

    void CameraSystem::Shutdown() {
    }
}
