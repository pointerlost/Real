//
// Created by pointerlost on 10/10/25.
//
#pragma once
#include "Math/Vec3.h"

namespace Real {
    struct LightSSBO;
    struct Transform;
}

namespace Real {

    class Light {
    public:
        enum class Mode : int {
            POINT,
            DIRECTIONAL,
            SPOT,
        };

    public:
        explicit Light(Mode type = Mode::POINT);
        ~Light() = default;
        Light(const Light&) = default;

        void SetRadiance(const math::Vec3& radiance) { m_Radiance = radiance;  }
        [[maybe_unused]] math::Vec3& GetRadiance()   { return m_Radiance;  }
        [[nodiscard]] math::Vec3 GetRadiance() const { return m_Radiance;  }

        void SetConstant(f32 constant) { m_Constant = constant; }
        [[nodiscard]] f32 GetConstant() const { return m_Constant; }
        void SetLinear(f32 linear) { m_Linear = linear; }
        [[nodiscard]] f32 GetLinear() const { return m_Linear; }
        void SetQuadratic(f32 quad) { m_Quadratic = quad; }
        [[nodiscard]] f32 GetQuadratic() const { return m_Quadratic; }

        void SetCutOff(f32 cutoff) { m_CutOff = cutoff; }
        void SetOuterCutOff(f32 outer) { m_OuterCutOff = outer; }
        [[nodiscard]] f32 GetCutOff() const { return m_CutOff; }
        [[nodiscard]] f32 GetOuterCutOff() const { return m_OuterCutOff; }

        [[nodiscard]] Mode GetType() const { return m_Type; }

        void Update(Transform& transform);
        void ConvertToGPUFormat(const Transform& transform, LightSSBO& outData);

    private:
        math::Vec3 m_Radiance = math::Vec3(1.0);

        // Attenuation parameters
        f32 m_Constant = 1.0;
        f32 m_Linear = 0.09;
        f32 m_Quadratic = 0.002;

        // Spot light
        f32 m_CutOff = 12.5;
        f32 m_OuterCutOff = 17.5;
        Mode m_Type = Mode::POINT; // point = 0, directional = 1, spot = 2
    };
}