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
#include "../../include/Timer/Timer.h"
#include "../../../apps/editor/include/EditorState.h"

namespace Real {

    CameraInput::CameraInput(Entity* camera) : m_Camera(camera){
    }

    void CameraInput::Update() {
        if (!Services::GetEditorState()->FpsMode) return;

        // TODO: add GetAllComponentsWithEntity function to manage easily
        auto& camera   = m_Camera->GetComponent<CameraComponent>().m_Camera;
        auto& movement = m_Camera->GetComponent<MovementComponent>();

        // Reset velocity speed
        movement.moveInput = math::Vec3{ 0.0, 0.0, 0.0 };

        // Input: Keyboard State
        if (Input::IsKeyHeld(REAL_KEY_W)) movement.moveInput.z += 15.0f;
        if (Input::IsKeyHeld(REAL_KEY_S)) movement.moveInput.z -= 15.0f;
        if (Input::IsKeyHeld(REAL_KEY_A)) movement.moveInput.x += 15.0f; // TODO: A and D working as opposite (Fix it)
        if (Input::IsKeyHeld(REAL_KEY_D)) movement.moveInput.x -= 15.0f;

        // Normalize diagonal input
        // Used a small epsilon 1e-6f instead of 0 to avoid tiny f32 errors
        if (math::Vec3::LengthSq(movement.moveInput) > 1e-6f)
            movement.moveInput = movement.moveInput.Normalized();

        // Input: Mouse Button State
        // do it some stuff

        // Mouse Scroll State
        if (Input::IsScrolling()) {
            camera.AddFOV(Input::GetScroll());
            Input::g_IsScrolling = false;
            Input::ResetScroll();
        }
    }

}
