//
// Created by pointerlost on 10/10/25.
//
#include "Graphics/Light.h"

#include "Core/Services.h"
#include "Core/Timer.h"
#include "Graphics/Transformations.h"
#include "Input/Input.h"
#include "Input/Keycodes.h"

namespace Real {

    Light::Light(LightType type) : m_Type(static_cast<int>(type)) {
    }

    void Light::Update(Transformations& transform) {
        if (Input::IsKeyHeld(REAL_KEY_KP_5)) {
            transform.AddTranslate(0.0, 0.1, 0.0);
        }
        if (Input::IsKeyHeld(REAL_KEY_KP_1)) {
            transform.AddTranslate(0.1, 0.0, 0.0);
        }
        if (Input::IsKeyHeld(REAL_KEY_KP_3)) {
            transform.AddTranslate(-0.1, 0.0, 0.0);
        }
        if (Input::IsKeyHeld(REAL_KEY_KP_2)) {
            transform.AddTranslate(0.0, -0.1, 0.0);
        }
        if (Input::IsKeyHeld(REAL_KEY_KP_4)) { // Z-axis
            transform.AddTranslate(0.0, 0.0, 0.1);
        }
        if (Input::IsKeyHeld(REAL_KEY_KP_6)) { // Z-axis
            transform.AddTranslate(0.0, 0.0, -0.1);
        }

        if (Input::IsKeyHeld(REAL_KEY_KP_SUBTRACT)) {
            transform.AddRotate(Services::GetEditorTimer()->GetDelta() * 45.0, glm::vec3(0.0, 1.0, 0.0));
        }
        if (Input::IsKeyHeld(REAL_KEY_KP_ADD)) {
            transform.AddRotate(Services::GetEditorTimer()->GetDelta() * 45.0, glm::vec3(1.0, 0.0, 0.0));
        }
        if (Input::IsKeyHeld(REAL_KEY_KP_ENTER)) {
            transform.AddRotate(Services::GetEditorTimer()->GetDelta() * 45.0, glm::vec3(0.0, 0.0, 1.0));
        }

        if (Input::IsKeyHeld(REAL_KEY_KP_7)) {
            m_Constant -= 0.001;
        }
        if (Input::IsKeyHeld(REAL_KEY_KP_8)) {
            m_Linear -= 0.001;
        }
        if (Input::IsKeyHeld(REAL_KEY_KP_9)) {
            m_Quadratic -= 0.001;
        }

        if (Input::IsKeyHeld(REAL_KEY_F1)) {
            m_Constant += 0.001;
        }
        if (Input::IsKeyHeld(REAL_KEY_F2)) {
            m_Linear += 0.001;
        }
        if (Input::IsKeyHeld(REAL_KEY_F3)) {
            m_Quadratic += 0.001;
        }
    }

    LightSSBO Light::ConvertToGPUFormat(Transformations& transform) {
        Update(transform);
        LightSSBO gpuData{};                                     // Convert angles to cosine
        gpuData.pos_cutoff = glm::vec4(transform.GetPosition(),  glm::cos(glm::radians(m_CutOff))); // Inner cone
        gpuData.dir_outer  = glm::vec4(transform.GetDirection(), glm::cos(glm::radians(m_OuterCutOff))); // Outer cone
        gpuData.diffuse   = glm::vec4(m_Diffuse, 1.0);
        gpuData.specular  = glm::vec4(m_Specular, 1.0);
        gpuData.constant  = m_Constant;
        gpuData.linear    = m_Linear;
        gpuData.quadratic = m_Quadratic;
        gpuData.type = m_Type;
        return gpuData;
    }
}
