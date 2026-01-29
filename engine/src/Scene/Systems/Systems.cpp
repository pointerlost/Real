//
// Created by pointerlost on 10/24/25.
//
#include "Scene/Systems/Systems.h"
#include "Scene/Systems/CameraSystem.h"
#include "Scene/Systems/PhysicsSystem.h"
#include "Scene/Systems/MeshRendererSystem.h"
#include "Scene/Systems/LightSystem.h"
#include "Scene/Systems/MovementSystem.h"

namespace Real {

    void Systems::Init() {
        m_SubSystems.push_back(CreateScope<ecs::CameraSystem>());
        m_SubSystems.push_back(CreateScope<ecs::PhysicsSystem>());
        m_SubSystems.push_back(CreateScope<ecs::MovementSystem>());
        m_SubSystems.push_back(CreateScope<ecs::MeshRendererSystem>());
        m_SubSystems.push_back(CreateScope<ecs::LightSystem>());

        // Init sub-systems resources
        for (const auto& ss : m_SubSystems) {
            ss->Init();
        }
    }

    void Systems::Update(Scene *scene, float deltaTime) {
        for (const auto& ss : m_SubSystems) {
            // Update sub-systems
            ss->Update(scene, deltaTime);
        }
    }

    void Systems::Shutdown() {
        for (const auto& ss : m_SubSystems) {
            ss->Shutdown();
        }
    }

    void Systems::SetRegistry(entt::registry &registry) {
        for (const auto& ss : m_SubSystems) {
            ss->SetRegistry(registry);
        }
    }
}
