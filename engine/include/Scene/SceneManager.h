//
// Created by pointerlost on 3/21/26.
//
#pragma once
#include "Scene.h"
#include "Core/Utils.h"

namespace Real {

    class SceneManager final {
    public:
        SceneManager();

        void SetActiveScene(Scope<Scene> scene) { m_ActiveScene = std::move(scene); }
        [[nodiscard]] Scene* GetActiveScene() const noexcept { return m_ActiveScene.get(); }

    private:
        Scope<Scene> m_ActiveScene;
    };
}