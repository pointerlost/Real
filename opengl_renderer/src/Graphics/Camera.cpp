//
// Created by pointerlost on 10/7/25.
//
#include "Graphics/Camera.h"
#include "Graphics/Transformations.h"
#include "Input/Input.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Real {

    Camera::Camera(CameraMode mode) : m_Mode(mode) {
    }

    void Camera::Update(Transformations& transform) {
        // Calculate forward(front) vector from yaw and pitch
        m_Forward.x = cos(glm::radians(Input::g_Yaw)) * cos(glm::radians(Input::g_Pitch));
        m_Forward.y = sin(glm::radians(Input::g_Pitch));
        m_Forward.z = sin(glm::radians(Input::g_Yaw)) * cos(glm::radians(Input::g_Pitch));
        m_Forward = glm::normalize(m_Forward);

        // Calculate right vector from world up
        m_Right = glm::normalize(glm::cross(m_Forward, glm::vec3(0.0, 1.0, 0.0)));

        // Recalculate Up
        m_Up = glm::normalize(glm::cross(m_Right, m_Forward));

        if (m_Mode == CameraMode::Perspective) {
            const auto& pos = transform.GetTranslate();
            m_View = glm::lookAt(pos, pos + m_Forward, m_WorldUp);
            // if (!m_ProjectionDirty) return;
            m_Projection = glm::perspective(glm::radians(m_FOV), m_Aspect, m_Near, m_Far);
            m_ProjectionDirty = false;
        }
        // TODO: Add orthographic camera

        // Set the transform quaternion for rotation to keep the camera direction in sync.
        transform.SetRotation(glm::quat(glm::rotation(glm::vec3(0,0,-1), m_Forward)));
    }

    CameraUBO Camera::ConvertToGPUFormat(Transformations& transform) {
        Update(transform);

        CameraUBO gpuData{};
        gpuData.position = glm::vec4(transform.GetTranslate(), 0.0); // w unused (padding)
        gpuData.view = m_View;
        gpuData.projection = m_Projection;
        gpuData.viewProjection = m_Projection * m_View;
        return gpuData;
    }
}
