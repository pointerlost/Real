//
// Created by pointerlost on 10/24/25.
//
#include "Scene/Systems.h"
#include "Scene/SystemUpdate.h"

namespace Real {

    void Systems::Init() {
        m_Updatables.push_back(CreateScope<CameraUpdate>());
        m_Updatables.push_back(CreateScope<TransformUpdate>());
        m_Updatables.push_back(CreateScope<VelocityUpdate>());
        m_Updatables.push_back(CreateScope<MeshRendererUpdate>());
        m_Updatables.push_back(CreateScope<LightUpdate>());
    }

    void Systems::UpdateAll(Scene *scene, float deltaTime) const {
        for (const auto& subSystem : m_Updatables) {
            // Update sub-systems
            subSystem->Update(scene, deltaTime);
        }
    }
}
