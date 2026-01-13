//
// Created by pointerlost on 10/7/25.
//
#pragma once
#include <glm/ext.hpp>
#include "../Core/RealConfig.h"
#include "GPUBuffers.h"
#include "Math/Mat4.h"

namespace Real {
    class Transform;
}

namespace Real {

    enum class CameraMode {
        Perspective,
        Orthographic
    };

    class Camera {
    public:
        explicit Camera(CameraMode mode = CameraMode::Perspective);

        void SetNear(float near) { m_Near = near; m_ProjectionDirty = true; }
        void SetFar(float far)   { m_Far = far;   m_ProjectionDirty = true; }
        void SetAspect(float aspect) { m_Aspect = aspect; m_ProjectionDirty = true; }
        void SetFOV(float fov) { m_FOV = fov; m_ProjectionDirty = true; }
        void AddFOV(float fov) {
            m_FOV += fov;
            if (m_FOV > 45.0) m_FOV = 45.0f;
            else if (m_FOV < 1.0) m_FOV = 1.0f;
            m_ProjectionDirty = true;
        }

        [[nodiscard]] float GetNear()   const { return m_Near;   }
        [[nodiscard]] float GetFar()    const { return m_Far;    }
        [[nodiscard]] float GetAspect() const { return m_Aspect; }
        [[nodiscard]] float GetFOV()    const { return m_FOV;    }

        [[nodiscard]] math::Mat4& GetView() { return m_View; }
        [[nodiscard]] math::Mat4& GetProjection() { return m_Projection; }
        [[nodiscard]] const math::Mat4& GetView() const { return m_View; }
        [[nodiscard]] const math::Mat4& GetProjection() const { return m_Projection; }

        void Update(Transform& transform);
        [[nodiscard]] CameraUBO ConvertToGPUFormat(const Transform& transform);

    private:
        float m_Near = 0.1;
        float m_Far  = 1000.0;
        float m_FOV  = 45.0;
        float m_Aspect = SCREEN_WIDTH / SCREEN_HEIGHT;
        CameraMode m_Mode = CameraMode::Perspective;

        const math::Vec3 m_SceneCenter = math::Vec3(0.0);
        const math::Vec3 m_WorldUp = math::Vec3(0.0, 1.0, 0.0);

        math::Mat4 m_View = math::Mat4(1.0);
        math::Mat4 m_Projection = math::Mat4(1.0);
        bool m_ProjectionDirty = true;
    };
}
