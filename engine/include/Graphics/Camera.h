//
// Created by pointerlost on 10/7/25.
//
#pragma once
#include <glm/ext.hpp>
#include "../Core/RealConfig.h"
#include "GPUBuffers.h"
#include "Math/Mat4.h"
#include "Math/Vec3.h"

namespace Real {
    struct Transform;
}

namespace Real {

    class Camera {
    public:
        enum class Mode {
            Perspective,
            Orthographic
        };

    public:
        explicit Camera(Mode mode = Mode::Perspective);

        void SetNear(f32 near) { m_Near = near; m_ProjectionDirty = true; }
        void SetFar(f32 far)   { m_Far = far;   m_ProjectionDirty = true; }
        void SetAspect(f32 aspect) { m_Aspect = aspect; m_ProjectionDirty = true; }
        void SetFOV(f32 fov) { m_FOV = fov; m_ProjectionDirty = true; }
        void AddFOV(f32 fov) {
            m_FOV += fov;
            if (m_FOV > 45.0) m_FOV = 45.0f;
            else if (m_FOV < 1.0) m_FOV = 1.0f;
            m_ProjectionDirty = true;
        }

        [[nodiscard]] f32 GetNear()   const { return m_Near;   }
        [[nodiscard]] f32 GetFar()    const { return m_Far;    }
        [[nodiscard]] f32 GetAspect() const { return m_Aspect; }
        [[nodiscard]] f32 GetFOV()    const { return m_FOV;    }

        [[nodiscard]] math::Mat4& GetView() { return m_View; }
        [[nodiscard]] math::Mat4& GetProjection() { return m_Projection; }
        [[nodiscard]] const math::Mat4& GetView() const { return m_View; }
        [[nodiscard]] const math::Mat4& GetProjection() const { return m_Projection; }
        [[nodiscard]] math::Mat4 GetViewProjection() const { return m_Projection * m_View; }

        void Update(Transform& transform);
        [[nodiscard]] FrameUBO ConvertToGPUFormat(const Transform& transform);

    private:
        f32 m_Near = 0.1;
        f32 m_Far  = 1000.0;
        f32 m_FOV  = 45.0;
        f32 m_Aspect = SCREEN_WIDTH / SCREEN_HEIGHT;
        Mode m_Mode = Mode::Perspective;

        const math::Vec3 m_SceneCenter = math::Vec3(0.0);
        const math::Vec3 m_WorldUp = math::Vec3(0.0, 1.0, 0.0);

        math::Mat4 m_View = math::Mat4(1.0);
        math::Mat4 m_Projection = math::Mat4(1.0);
        bool m_ProjectionDirty = true;
    };
}
