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

        void AddDiffuse(const glm::vec3& diffuse) { m_Diffuse += diffuse; }
        void SetDiffuse(const glm::vec3& diffuse) { m_Diffuse = diffuse;  }
        void AddSpecular(const glm::vec3& specular) { m_Specular += specular; }
        void SetSpecular(const glm::vec3& specular) { m_Specular = specular;  }
        [[nodiscard]] const glm::vec3& GetDiffuse()  const { return m_Diffuse;  }
        [[nodiscard]] const glm::vec3& GetSpecular() const { return m_Specular; }

        void Update(Transformations& transform);
        [[nodiscard]] LightSSBO ConvertToGPUFormat(Transformations& transform);

    private:
        // TODO: PBR lighting
        glm::vec3 m_Diffuse  = glm::vec3(1.0);
        glm::vec3 m_Specular = glm::vec3(1.0);

        // Attenuation parameters
        float m_Constant = 1.0;
        float m_Linear = 0.09;
        float m_Quadratic = 0.032;

        // Spot light
        float m_CutOff = 12.5;
        float m_OuterCutOff = 17.5;
        int m_Type = static_cast<int>(LightType::POINT); // point = 0, directional = 1, spot = 2
    };
}