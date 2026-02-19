//
// Created by pointerlost on 2/16/26.
//
#include <Core/Engine/EngineCore.h>

#include "Core/Services.h"
#include "Core/Utils.h"
#include "Core/Engine/EngineBootstrap.h"
#include "../../../../apps/editor/include/Editor.h"
#include "Physics/Physx/PhysXBackend.h"
#include "Scene/Components.h"
#include "Scene/Systems/CameraSystem.h"
#include "Scene/Systems/LightSystem.h"
#include "Scene/Systems/MeshRendererSystem.h"
#include "Scene/Systems/MovementSystem.h"
#include "Scene/Systems/PhysicsSystem.h"
#include "Timer/Timer.h"

namespace Real::core {

    EngineCore::EngineCore(const EngineConfig &cfg)
    {
    }

    void EngineCore::Start() {
        m_Timer->Start();
    }

    void EngineCore::RunLoop() {
    }

    void EngineCore::Stop() {
        m_Timer->Stop();
    }
}
