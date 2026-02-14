//
// Created by pointerlost on 10/7/25.
//
#include "Graphics/Camera.h"
#include "Graphics/Transformations.h"
#include "Input/Input.h"
#include "Math/Conversions/GLMConvertions.h"

namespace Real {

    Camera::Camera(Mode mode) : m_Mode(mode) {
    }

    void Camera::Update(Transform& transform) {
        // Clamp pitch
        Input::g_Pitch = std::clamp(Input::g_Pitch, -89.0f, 89.0f);

        // Build rotation from yaw and pitch
        const math::Quat yaw   = math::Quat::FromAxisAngle(math::Vec3(0, 1, 0), math::DegreesToRadians(Input::g_Yaw));
        const math::Quat pitch = math::Quat::FromAxisAngle(math::Vec3(1, 0, 0), math::DegreesToRadians(Input::g_Pitch));

        const math::Quat rotation = yaw * pitch;

        // Write rotation into transform
        transform.rotation = rotation;

        // Read directions from transform
        const glm::vec3 position = interop::glm::To(transform.position);
        const glm::vec3 forward  = interop::glm::To(transform.Forward());
        const glm::vec3 up       = interop::glm::To(transform.Up());

        // Build view matrix
        m_View = interop::glm::From(glm::lookAt(position, position + forward, up));

        // Build Projection matrix
        if (m_Mode == Mode::Perspective && m_ProjectionDirty) {
            m_Projection = interop::glm::From(glm::perspective(math::DegreesToRadians(m_FOV), m_Aspect, m_Near, m_Far));
            m_ProjectionDirty = false;
        }

        // TODO: Add orthographic camera
    }

    FrameUBO Camera::ConvertToGPUFormat(const Transform& transform) {
        FrameUBO gpuData{};
        gpuData.position = math::Vec4(transform.position, 0.0f); // w unused (padding)
        gpuData.view = m_View;
        gpuData.projection = m_Projection;
        gpuData.viewProjection = m_Projection * m_View;
        return gpuData;
    }
}
