//
// Created by pointerlost on 3/21/26.
//
#include "Scene/SceneManager.h"

namespace Real {

    SceneManager::SceneManager()
        : m_ActiveScene(CreateScope<Scene>())
    {
    }
}
