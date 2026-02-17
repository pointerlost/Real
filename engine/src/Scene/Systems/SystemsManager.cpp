//
// Created by pointerlost on 10/24/25.
//
#include "Scene/Systems/SystemsManager.h"
#include "Scene/ISceneListener.h"
#include <cassert>

#include "Event/SceneEvents.h"

namespace Real::core {

    void SystemManager::OnSceneAttach(entt::registry& registry, event::SceneEvents& events) {
        for (auto& s : m_Systems)
            if (auto* listener = dynamic_cast<ISceneListener*>(s.get()))
                listener->OnSceneAttach(registry, events);
    }

    void SystemManager::OnSceneDetach(entt::registry& registry, event::SceneEvents& events) {
        for (auto& s : m_Systems)
            if (auto* listener = dynamic_cast<ISceneListener*>(s.get()))
                listener->OnSceneDetach(registry, events);
    }

    void SystemManager::Init() {
        // Init sub-systems resources
        for (const auto& ss : m_Systems) {
            ss->Init();
        }
    }

    void SystemManager::Update(entt::registry& registry, f32 dt) {
        for (const auto& ss : m_Systems) {
            // Update sub-systems
            ss->Update(registry, dt);
        }
    }

    void SystemManager::Shutdown() {
        for (const auto& ss : m_Systems) {
            ss->Shutdown();
        }
    }

    void SystemManager::AddSystem(Scope<ISystem> system) {
        // only non-null systems
        assert(system);
        m_Systems.push_back(std::move(system));
    }
}
