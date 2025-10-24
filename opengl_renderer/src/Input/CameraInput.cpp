//
// Created by pointerlost on 10/8/25.
//
#include "Input/CameraInput.h"
#include "Input/Input.h"
#include "Input/Keycodes.h"
#include "Scene/Components.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "Core/Services.h"
#include "Core/Timer.h"
#include "Editor/EditorState.h"

namespace Real {

    CameraInput::CameraInput(Entity* camera) : m_Camera(camera){
    }

    void CameraInput::Update() {
        if (!Services::GetEditorState()->fpsMode) return;

        // TODO: add GetAllComponentsWithEntity function to manage easily
        auto& camera = m_Camera->GetComponent<CameraComponent>()->m_Camera;
        auto velocity = m_Camera->GetComponent<VelocityComponent>();
        const auto& deltaTime = Services::GetEditorTimer()->GetDelta();

        // Reset velocity speed
        *velocity = VelocityComponent{};

        // Input: Keyboard State
        if (Input::IsKeyHeld(REAL_KEY_W)) {
            velocity->m_Speed.z += 15.0;
        }
        if (Input::IsKeyHeld(REAL_KEY_A)) {
            velocity->m_Speed.x -= 15.0;
        }
        if (Input::IsKeyHeld(REAL_KEY_S)) {
            velocity->m_Speed.z -= 15.0;
        }
        if (Input::IsKeyHeld(REAL_KEY_D)) {
            velocity->m_Speed.x += 15.0;
        }

        // Input: Mouse Button State
        // do it some stuff

        // Mouse Scroll State
        if (Input::IsScrolling()) {
            camera.AddFOV(Input::GetScroll() * deltaTime);
            Input::g_IsScrolling = false;
            Input::ResetScroll();
        }
    }

}
