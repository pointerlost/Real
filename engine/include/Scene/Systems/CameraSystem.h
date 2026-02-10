//
// Created by pointerlost on 1/14/26.
//
#pragma once
#include "Systems.h"

namespace Real::ecs {

    class CameraSystem final : public Systems {
        void Init() override;
        void Update(Scene *scene, float deltaTime) override;
        void Shutdown() override;
        void SetRegistry(entt::registry& registry) override;

        void OnSceneAttach(Scene* scene) override;
        void OnSceneDetach(Scene* scene) override;

    private:
        entt::registry* m_Registry = nullptr;
    };
}
