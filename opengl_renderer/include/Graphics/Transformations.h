//
// Created by pointerlost on 10/6/25.
//
#pragma once
#include <glm/ext.hpp>

#include "GPUBuffers.h"

namespace Real {

    class Transformations {
    public:
        Transformations(const glm::vec3& translate, const glm::vec3& rotate, const glm::vec3& scale);
        Transformations(const glm::vec3& translate, const glm::quat& rotate, const glm::vec3& scale);
        Transformations() = default;
        ~Transformations() = default;
        Transformations(Transformations&) = default;

        [[nodiscard]] const glm::mat4& GetModelMatrix() const { return m_ModelMatrix; }
        [[nodiscard]] glm::mat3 GetNormalMatrix() const { return glm::transpose(glm::inverse(glm::mat3(m_ModelMatrix))); }

        void AddTranslate(const glm::vec3& position) { m_Translate += position; m_ModelMatrixDirty = true; }
        void AddTranslate(float x, float y, float z) { m_Translate += glm::vec3(x, y, z); m_ModelMatrixDirty = true; }
        void SetTranslate(const glm::vec3& position) { m_Translate = position;  m_ModelMatrixDirty = true; }
        [[nodiscard]] const glm::vec3& GetPosition() const { return m_Translate; }

        // Add rotations
        void AddRotate(const glm::mat4& rotate) { m_Rotate += glm::quat_cast(rotate); m_ModelMatrixDirty = true; }
        void AddRotate(const glm::quat& rotate) { m_Rotate += rotate; m_ModelMatrixDirty = true; }
        void SetRotate(const glm::mat4& rotate) { m_Rotate = glm::quat_cast(rotate); m_ModelMatrixDirty = true; }
        void SetRotate(const glm::quat& rotate) { m_Rotate = rotate; m_ModelMatrixDirty = true; }
        [[nodiscard]] const glm::quat& GetRotationWithQuat() const { return m_Rotate; }
        [[nodiscard]] glm::mat4 GetRotationWithMat4() const { return glm::mat4_cast(m_Rotate); }

        void AddScale(const glm::vec3& scale) { m_Scale += scale; m_ModelMatrixDirty = true; }
        void AddScale(float x, float y, float z) { m_Scale += glm::vec3(x, y, z); m_ModelMatrixDirty = true; }
        void SetScale(const glm::vec3& scale) { m_Scale = scale;  m_ModelMatrixDirty = true; }
        [[nodiscard]] const glm::vec3& GetScale() const { return m_Scale; }

        void Update();
        [[nodiscard]] TransformSSBO ConvertToGPUFormat();

    private:
        glm::vec3 m_Translate = glm::vec3(0.0f);
        glm::quat m_Rotate = glm::identity<glm::quat>();
        glm::vec3 m_Scale = glm::vec3(1.0f);

        glm::mat4 m_ModelMatrix = glm::mat4(1.0f);
        mutable bool m_ModelMatrixDirty = true;
    };
}
