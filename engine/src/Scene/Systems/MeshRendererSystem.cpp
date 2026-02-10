//
// Created by pointerlost on 1/14/26.
//
#include <Scene/Systems/MeshRendererSystem.h>

namespace Real::ecs {

    void MeshRendererSystem::Init() {
    }

    void MeshRendererSystem::Update(Scene *scene, float deltaTime) {
    }

    void MeshRendererSystem::Shutdown() {
    }

    void MeshRendererSystem::SetRegistry(entt::registry &registry) {
        m_Registry = &registry;
    }

    void MeshRendererSystem::OnSceneAttach(Scene *scene) {
    }

    void MeshRendererSystem::OnSceneDetach(Scene *scene) {
    }
}
