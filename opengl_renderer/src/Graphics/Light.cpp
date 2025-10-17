//
// Created by pointerlost on 10/10/25.
//
#include "Graphics/Light.h"
#include "Graphics/Transformations.h"
#include "Input/Input.h"
#include "Input/Keycodes.h"

namespace Real {

    Light::Light() {
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
        LightSSBO gpuData{};
        gpuData.diffuse  = glm::vec4(m_Diffuse, 1.0);
        gpuData.specular = glm::vec4(m_Specular, 1.0);
        gpuData.position = glm::vec4(transform.GetPosition(), 1.0);
        gpuData.constLinQuadratic = glm::vec4(m_Constant, m_Linear, m_Quadratic, 1.0);
        return gpuData;
    }
}
