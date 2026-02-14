//
// Created by pointerlost on 10/24/25.
//
#include "Scene/Systems/Systems.h"
#include "Scene/ISceneListener.h"
#include <cassert>

namespace Real {

    void Systems::OnSceneAttach(Scene *scene) {
        for (auto& s : m_SubSystems)
            if (auto* listener = dynamic_cast<ISceneListener*>(s.get()))
                listener->OnSceneAttach(scene);
    }

    void Systems::OnSceneDetach(Scene *scene) {
        for (auto& s : m_SubSystems)
            if (auto* listener = dynamic_cast<ISceneListener*>(s.get()))
                listener->OnSceneDetach(scene);
    }

    void Systems::Init() {
        // Init sub-systems resources
        for (const auto& ss : m_SubSystems) {
            ss->Init();
        }
    }

    void Systems::Update(Scene *scene, f32 deltaTime) {
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

    void Systems::AddSystem(Scope<ISystem> system) {
        // only non-null systems
        assert(system);
        m_SubSystems.push_back(std::move(system));
    }
}
