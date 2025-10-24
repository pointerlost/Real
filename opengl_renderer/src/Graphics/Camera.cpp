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
        glm::vec3 forward(0.0);
        forward.x = cos(glm::radians(Input::g_Yaw)) * cos(glm::radians(Input::g_Pitch));
        forward.y = sin(glm::radians(Input::g_Pitch));
        forward.z = sin(glm::radians(Input::g_Yaw)) * cos(glm::radians(Input::g_Pitch));
        forward = glm::normalize(forward);
        transform.SetWorldDirection(forward);

        // Calculate right vec
        const glm::vec3& right = glm::normalize(glm::cross(forward, glm::vec3(0.0, 1.0, 0.0)));
        transform.SetRight(glm::normalize(glm::cross(forward, glm::vec3(0.0, 1.0, 0.0))));

        // Calculate Up vec
        transform.SetUp(glm::normalize(glm::cross(right, forward)));

        // Calculate forward vector from yaw and pitch angles, then convert to quat rotation
        // This ensures sync quaternion representation
        transform.SetRotation(forward);

        if (m_Mode == CameraMode::Perspective) {
            const auto& pos = transform.GetTranslate();
            m_View = glm::lookAt(pos, pos + forward, m_WorldUp);
            if (!m_ProjectionDirty) return;
            m_Projection = glm::perspective(glm::radians(m_FOV), m_Aspect, m_Near, m_Far);
            m_ProjectionDirty = false;
        }

        // TODO: Add orthographic camera
    }

    CameraUBO Camera::ConvertToGPUFormat(Transformations& transform) {
        CameraUBO gpuData{};
        gpuData.position = glm::vec4(transform.GetTranslate(), 0.0); // w unused (padding)
        gpuData.view = m_View;
        gpuData.projection = m_Projection;
        gpuData.viewProjection = m_Projection * m_View;
        return gpuData;
    }
}
