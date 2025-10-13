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

namespace Real {

    CameraInput::CameraInput(Entity* camera) : m_Camera(camera){
    }

    void CameraInput::Update() {
        auto& camera = m_Camera->GetComponent<CameraComponent>()->m_Camera;
        auto& transform = m_Camera->GetComponent<TransformComponent>()->m_Transform;
        // Input: Keyboard State
        const auto& direction = camera.GetDirection();
        const auto& right = camera.GetRight();
        constexpr float translate = 0.25f;
        if (Input::IsKeyHeld(REAL_KEY_W)) {
            transform.AddTranslate(direction * translate);
        }
        if (Input::IsKeyHeld(REAL_KEY_A)) {
            transform.AddTranslate(-right * translate);
        }
        if (Input::IsKeyHeld(REAL_KEY_S)) {
            transform.AddTranslate(-direction * translate);
        }
        if (Input::IsKeyHeld(REAL_KEY_D)) {
            transform.AddTranslate(right * translate);
        }
        // Input: Mouse Button State
        // do it some stuff

        // Mouse Scroll State
        if (Input::IsScrolling()) {
            camera.AddFOV(Input::GetScroll() * Services::GetEditorTimer()->GetDelta());
            Input::g_IsScrolling = false;
            Input::ResetScroll();
        }

        transform.Update();
    }

}
