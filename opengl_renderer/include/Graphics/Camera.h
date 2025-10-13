//
// Created by pointerlost on 10/7/25.
//
#pragma once
#include <glm/ext.hpp>
#include "Config.h"
#include "GPUBuffers.h"

namespace Real {
    class Transformations;
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

        void SetDirection(const glm::vec3& dir) { m_Forward = dir; }
        void SetUp(const glm::vec3& up) { m_Up = up; }
        void SetRight(const glm::vec3& right) { m_Right = right; }
        [[nodiscard]] const glm::vec3& GetDirection() const { return m_Forward; }
        [[nodiscard]] const glm::vec3& GetUp() const { return m_Up; }
        [[nodiscard]] const glm::vec3& GetRight() const { return m_Right; }

        [[nodiscard]] const glm::mat4& GetView() const { return m_View; }
        [[nodiscard]] const glm::mat4& GetProjection() const { return m_Projection; }

        void Update(Transformations& transform);
        [[nodiscard]] CameraUBO ConvertToGPUFormat() const;

    private:
        float m_Near = 0.01f;
        float m_Far  = 100.0f;
        float m_FOV  = 45.0f;
        float m_Aspect = SCREEN_WIDTH / SCREEN_HEIGHT;
        CameraMode m_Mode = CameraMode::Perspective;

        const glm::vec3 m_SceneCenter = glm::vec3(0.0f);
        const glm::vec3 m_WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

        glm::vec3 m_Forward = glm::vec3(0.0, 0.0, -1.0);
        glm::vec3 m_Up = glm::vec3(0.0, 1.0, 0.0);
        glm::vec3 m_Right = glm::vec3(1.0, 0.0, 0.0);

        glm::mat4 m_View = glm::mat4(1.0f);
        glm::mat4 m_Projection = glm::mat4(1.0f);
        bool m_ProjectionDirty = true;
    };
}
