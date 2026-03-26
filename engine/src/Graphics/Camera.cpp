//
// Created by pointerlost on 10/7/25.
//
#include "Graphics/Camera.h"

#include "Graphics/GPUBuffers.h"
#include "Graphics/Transformations.h"
#include "Input/Input.h"
#include "Math/Math.h"
#include "Math/Conversions/GLMConversions.h"

namespace Real {

    Camera::Camera(Mode mode) : m_Mode(mode) {
    }

    void Camera::Update(Transform& transform) {
        // Clamp pitch
        Input::g_Pitch = std::clamp(Input::g_Pitch, -89.0f, 89.0f);

        // Build rotation from yaw and pitch
        const math::Quat yaw   = math::Quat::FromAxisAngle(math::Vec3(0, 1, 0), math::DegreesToRadians(Input::g_Yaw)  );
        const math::Quat pitch = math::Quat::FromAxisAngle(math::Vec3(1, 0, 0), math::DegreesToRadians(Input::g_Pitch));

        const math::Quat rotation = yaw * pitch;

        // Write rotation into transform
        transform.SetLocalRotation(rotation);

        // Read directions from transform
        const glm::vec3 position = interop::glm::To(transform.GetWorldPosition());
        const glm::vec3 forward  = interop::glm::To(transform.Forward());
        const glm::vec3 up       = interop::glm::To(transform.Up());

        // Build view matrix
        m_View = interop::glm::From(glm::lookAt(position, position + forward, up));

        // Build Projection matrix
        if (m_Mode == Mode::Perspective && m_ProjectionDirty) {
            m_Projection = interop::glm::From(
                glm::perspective(math::DegreesToRadians(m_FOV), m_Aspect, m_Near, m_Far)
            );
            m_ProjectionDirty = false;
        }

        // TODO: Add orthographic camera
    }

    void Camera::ConvertToGPUFormat(const Transform& transform, FrameUBO& outData) const {
        outData.position       = math::Vec4(transform.GetWorldPosition(), 0.0f); // w unused (padding)
        outData.view           = m_View;
        outData.projection     = m_Projection;
        outData.viewProjection = m_Projection * m_View;
    }
}
