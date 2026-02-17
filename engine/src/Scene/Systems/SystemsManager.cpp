//
// Created by pointerlost on 10/24/25.
//
#include "Scene/Systems/Systems.h"
#include "Scene/ISceneListener.h"
#include <cassert>

namespace Real {

    void SystemManager::OnSceneAttach(IScene *scene) {
        for (auto& s : m_Systems)
            if (auto* listener = dynamic_cast<ISceneListener*>(s.get()))
                listener->OnSceneAttach(scene);
    }

    void SystemManager::OnSceneDetach(IScene *scene) {
        for (auto& s : m_Systems)
            if (auto* listener = dynamic_cast<ISceneListener*>(s.get()))
                listener->OnSceneDetach(scene);
    }

    void SystemManager::Init() {
        // Init sub-systems resources
        for (const auto& ss : m_Systems) {
            ss->Init();
        }
    }

    void SystemManager::Update(IScene *scene, f32 dt) {
        for (const auto& ss : m_Systems) {
            // Update sub-systems
            ss->Update(scene, dt);
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
