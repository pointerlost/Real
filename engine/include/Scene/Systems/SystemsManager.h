//
// Created by pointerlost on 10/24/25.
//
#pragma once
#include "Core/ISystemManager.h"

namespace Real { class IScene; }

namespace Real {

    class SystemManager final {
    public:
        SystemManager() = default;
        SystemManager(const SystemManager&) = delete;
        SystemManager& operator=(const SystemManager&) = delete;

    public:
        void AddSystem(Scope<ISystem> system);

        void Init();
        void Update(IScene* scene, f32 dt);
        void Shutdown();

        void OnSceneAttach(IScene* scene);
        void OnSceneDetach(IScene* scene);

    private:
        Vector<Scope<ISystem>> m_Systems;
    };

}
