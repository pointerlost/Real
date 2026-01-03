//
// Created by pointerlost on 10/24/25.
//
#include "Scene/SystemUpdate.h"
#include "Scene/Components.h"
#include "Scene/Scene.h"

namespace Real {

    void TransformUpdate::Update(Scene *scene, float deltaTime) {
        const auto& view = scene->GetAllEntitiesWith<TransformComponent>();

        for (const auto& [entity, transform] : view.each()) {
            transform.m_Transform.Update();
        }
    }

    void VelocityUpdate::Update(Scene *scene, float deltaTime) {
        const auto& view = scene->GetAllEntitiesWith<VelocityComponent, TransformComponent>();

        for (const auto& [entity, vc, tc] : view.each()) {
            auto& transform = tc.m_Transform;

            vc.m_LinearVelocity = (
                transform.GetRight() * vc.m_Speed.x +
                transform.GetUp()    * vc.m_Speed.y +
                transform.GetWorldDirection() * vc.m_Speed.z
            );

            // TODO: need movement system to update transform stuff
            transform.AddTranslate(vc.m_LinearVelocity);

            // TODO: Add acceleration for rotation
        }
    }

    void MeshRendererUpdate::Update(Scene *scene, float deltaTime) {
    }

    void CameraUpdate::Update(Scene *scene, float deltaTime) {
        const auto& view = scene->GetAllEntitiesWith<CameraComponent, TransformComponent>();

        for (const auto& [entity, camera, transform] : view.each()) {
            camera.m_Camera.Update(transform.m_Transform);
        }
    }

    void LightUpdate::Update(Scene *scene, float deltaTime) {
        const auto& view = scene->GetAllEntitiesWith<LightComponent, TransformComponent>();

        for (const auto& [entity, light, transform] : view.each()) {
            light.m_Light.Update(transform.m_Transform);
        }
    }
}
