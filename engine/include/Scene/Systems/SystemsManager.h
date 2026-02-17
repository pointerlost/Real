//
// Created by pointerlost on 10/24/25.
//
#pragma once
#include "Core/ISystem.h"
#include "Core/Utils.h"

namespace Real {
    namespace event {
        class SceneEvents;
    }

    class Scene; }

namespace Real::core {

    class SystemManager final {
    public:
        SystemManager() = default;
        SystemManager(const SystemManager&) = delete;
        SystemManager& operator=(const SystemManager&) = delete;

    public:
        void AddSystem(Scope<ISystem> system);

        void Init();
        void Update(entt::registry& registry, f32 dt);
        void Shutdown();

        void OnSceneAttach(entt::registry& registry, event::SceneEvents& events);
        void OnSceneDetach(entt::registry& registry, event::SceneEvents& events);

    private:
        Vector<Scope<ISystem>> m_Systems;
    };

}
