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

        void SetModelMatrix(const glm::mat4& model) { m_ModelMatrix = model; }
        // To update gizmos
        [[nodiscard]] glm::mat4& GetModelMatrix() { m_ModelMatrixDirty = false; return m_ModelMatrix; }
        [[nodiscard]] const glm::mat4& GetModelMatrix() const { return m_ModelMatrix; }
        [[nodiscard]] glm::mat3 GetNormalMatrix() const { return glm::transpose(glm::inverse(glm::mat3(m_ModelMatrix))); }

        void AddTranslate(const glm::vec3& position) { m_Translate += position; m_ModelMatrixDirty = true; }
        void AddTranslate(float x, float y, float z) { m_Translate += glm::vec3(x, y, z); m_ModelMatrixDirty = true; }
        void SetTranslate(const glm::vec3& position) { m_Translate = position;  m_ModelMatrixDirty = true; }
        [[nodiscard]] const glm::vec3& GetTranslate() const { return m_Translate; }
        [[nodiscard]] glm::vec3& GetTranslate() { m_ModelMatrixDirty = true; return m_Translate; }

        [[nodiscard]] glm::vec3 GetDirection() const {
            const glm::vec3 forward = m_Rotate * glm::vec3(0.0, 0.0, -1.0);
            return glm::normalize(glm::vec3{ forward.x, forward.y, forward.z});
        }

        // Add rotations
        void AddRotate(float angle, const glm::vec3& axis) {
            m_Rotate = glm::angleAxis(glm::radians(angle), axis) * m_Rotate;
            m_ModelMatrixDirty = true;
        }
        void SetRotate(float angle, const glm::vec3& axis) {
            m_Rotate = glm::angleAxis(glm::radians(angle), axis);
            m_ModelMatrixDirty = true;
        }
        void SetRotationEuler(const glm::vec3& eulerDegrees) {
            m_Rotate = glm::quat(glm::radians(eulerDegrees));
        }
        void SetRotation(const glm::quat& rotate) { m_ModelMatrixDirty = true; m_Rotate = rotate; }
        void SetRotation(const glm::mat4& rotate) { m_ModelMatrixDirty = true; m_Rotate = glm::quat_cast(rotate); }
        [[nodiscard]] glm::vec3 GetRotationEuler() const { m_ModelMatrixDirty = true; return glm::degrees(glm::eulerAngles(m_Rotate)); }
        [[nodiscard]] const glm::quat& GetRotationWithQuat() const { return m_Rotate; }
        [[nodiscard]] glm::mat4 GetRotationWithMat4() const { return glm::mat4_cast(m_Rotate); }

        void AddScale(const glm::vec3& scale) { m_Scale += scale; m_ModelMatrixDirty = true; }
        void AddScale(float x, float y, float z) { m_Scale += glm::vec3(x, y, z); m_ModelMatrixDirty = true; }
        void SetScale(const glm::vec3& scale) { m_Scale = scale;  m_ModelMatrixDirty = true; }
        [[nodiscard]] const glm::vec3& GetScale() const { return m_Scale; }
        [[nodiscard]] glm::vec3& GetScale() { m_ModelMatrixDirty = true; return m_Scale; }

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
