//
// Created by pointerlost on 1/14/26.
//
#include <Scene/Systems/CameraSystem.h>
#include "Scene/Components.h"
#include "Scene/Scene.h"

namespace Real::ecs {

    void CameraSystem::Init() {
    }

    void CameraSystem::Update(Scene *scene, float deltaTime) {
        const auto& view = scene->GetAllEntitiesWith<CameraComponent, TransformComponent>();

        for (const auto& [entity, camera, transform] : view.each()) {
            camera.m_Camera.Update(transform.transform);
        }
    }

    void CameraSystem::Shutdown() {
    }
}
