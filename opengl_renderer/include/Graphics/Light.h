//
// Created by pointerlost on 10/10/25.
//
#pragma once
#include <glm/ext.hpp>

#include "GPUBuffers.h"

namespace Real {
    class Transformations;
}

namespace Real {

    enum class LightType : int {
        POINT,
        DIRECTIONAL,
        SPOT,
    };

    class Light {
    public:
        explicit Light(LightType type = LightType::POINT);
        ~Light() = default;
        Light(const Light&) = default;

        void SetRadiance(const glm::vec3& radiance) { m_Radiance = radiance;  }
        [[maybe_unused]] glm::vec3& GetRadiance()   { return m_Radiance;  }
        [[nodiscard]] glm::vec3 GetRadiance() const { return m_Radiance;  }

        void SetConstant(float constant) { m_Constant = constant; }
        [[nodiscard]] float GetConstant() const { return m_Constant; }
        void SetLinear(float linear) { m_Linear = linear; }
        [[nodiscard]] float GetLinear() const { return m_Linear; }
        void SetQuadratic(float quad) { m_Quadratic = quad; }
        [[nodiscard]] float GetQuadratic() const { return m_Quadratic; }

        void SetCutOff(float cutoff) { m_CutOff = cutoff; }
        void SetOuterCutOff(float outer) { m_OuterCutOff = outer; }
        [[nodiscard]] float GetCutOff() const { return m_CutOff; }
        [[nodiscard]] float GetOuterCutOff() const { return m_OuterCutOff; }

        [[nodiscard]] LightType GetType() const { return m_Type; }

        void Update(Transformations& transform);
        [[nodiscard]] LightSSBO ConvertToGPUFormat(Transformations& transform);

    private:
        glm::vec3 m_Radiance  = glm::vec3(1.0);

        // Attenuation parameters
        float m_Constant = 1.0;
        float m_Linear = 0.09;
        float m_Quadratic = 0.002;

        // Spot light
        float m_CutOff = 12.5;
        float m_OuterCutOff = 17.5;
        LightType m_Type = LightType::POINT; // point = 0, directional = 1, spot = 2
    };
}