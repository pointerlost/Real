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

    class Light {
    public:
        Light();
        ~Light() = default;
        Light(const Light&) = default;

        void AddDiffuse(const glm::vec3& diffuse) { m_Diffuse += diffuse; }
        void SetDiffuse(const glm::vec3& diffuse) { m_Diffuse = diffuse;  }
        void AddSpecular(const glm::vec3& specular) { m_Specular += specular; }
        void SetSpecular(const glm::vec3& specular) { m_Specular = specular;  }
        [[nodiscard]] const glm::vec3& GetDiffuse()  const { return m_Diffuse;  }
        [[nodiscard]] const glm::vec3& GetSpecular() const { return m_Specular; }

        void Update(Transformations& transform);
        [[nodiscard]] LightSSBO ConvertToGPUFormat() const;

    private:
        // TODO: go to PBR slowly!!!
        glm::vec3 m_Diffuse = glm::vec3(1.0f);
        glm::vec3 m_Specular = glm::vec3(0.6f);
    };
}