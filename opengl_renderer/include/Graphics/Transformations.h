//
// Created by pointerlost on 10/6/25.
//
#pragma once
#include <glm/ext.hpp>

#include "GPUBuffers.h"

namespace Real {

    class Transformations {
    public:
        // TODO: implement it
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
        void SetTranslate(const glm::vec3& position) { m_Translate = position;  m_ModelMatrixDirty = true; }
        [[nodiscard]] const glm::vec3& GetTranslate() const { return m_Translate; }
        [[nodiscard]] glm::vec3 GetTranslate() { return m_Translate; }

        // World-space direction
        void SetWorldDirection(const glm::vec3& dir) { m_ModelMatrixDirty = true; m_WorldForward = dir; }
        [[nodiscard]] glm::vec3 GetWorldDirection() const {
            return glm::normalize(glm::mat3(m_ModelMatrix) * glm::vec3(0.0, 0.0, -1.0));
        }

        void SetLocalDirection(const glm::vec3& dir) { m_ModelMatrixDirty = true; m_LocalForward = dir; }
        [[nodiscard]] glm::vec3 GetLocalDirection() const {
            return glm::normalize(glm::mat3(m_Rotate) * glm::vec3(0.0, 0.0, -1.0));
        }

        void SetUp(const glm::vec3& up) { m_Up = up; }
        void SetRight(const glm::vec3& right) { m_Right = right; }
        [[nodiscard]] const glm::vec3& GetUp() const { return m_Up; }
        [[nodiscard]] const glm::vec3& GetRight() const { return m_Right; }

        void AddRotate(float angle, const glm::vec3& axis) {
            m_ModelMatrixDirty = true;
            m_Rotate = glm::angleAxis(glm::radians(angle), axis) * m_Rotate;
        }
        void SetRotate(float angle, const glm::vec3& axis) {
            m_ModelMatrixDirty = true;
            m_Rotate = glm::angleAxis(glm::radians(angle), axis);
        }
        void SetRotationEuler(const glm::vec3& eulerDegrees) {
            m_ModelMatrixDirty = true;
            m_Rotate = glm::quat(glm::radians(eulerDegrees));
        }
        void SetRotation(const glm::quat& rotate) { m_ModelMatrixDirty = true; m_Rotate = rotate; }
        void SetRotation(const glm::mat4& rotate) { m_ModelMatrixDirty = true; m_Rotate = glm::quat_cast(rotate); }
        void SetRotation(const glm::vec3& forward) { m_ModelMatrixDirty = true; m_Rotate = glm::quat(glm::vec3(0, 0, -1), forward); }
        [[nodiscard]] glm::vec3 GetRotationEuler() const { return glm::degrees(glm::eulerAngles(m_Rotate)); }
        [[nodiscard]] const glm::quat& GetRotationWithQuat() const { return m_Rotate; }
        [[nodiscard]] glm::mat4 GetRotationWithMat4() const { return glm::mat4_cast(m_Rotate); }

        void AddScale(const glm::vec3& scale) { m_Scale += scale; m_ModelMatrixDirty = true; }
        void SetScale(const glm::vec3& scale) { m_Scale = scale;  m_ModelMatrixDirty = true; }
        [[nodiscard]] const glm::vec3& GetScale() const { return m_Scale; }
        [[nodiscard]] glm::vec3 GetScale() { return m_Scale; }

        void Update();
        [[nodiscard]] TransformSSBO ConvertToGPUFormat();

    private:
        glm::vec3 m_Translate = glm::vec3(0.0f);
        glm::quat m_Rotate = glm::identity<glm::quat>();
        glm::vec3 m_Scale = glm::vec3(1.0f);

        glm::vec3 m_LocalForward = glm::vec3(0.0, 0.0, -1.0);
        glm::vec3 m_WorldForward = glm::vec3(0.0, 0.0, -1.0);
        glm::vec3 m_Up = glm::vec3(0.0, 1.0, 0.0);
        glm::vec3 m_Right = glm::vec3(1.0, 0.0, 0.0);

        glm::mat4 m_ModelMatrix = glm::mat4(1.0f);
        mutable bool m_ModelMatrixDirty = true;
    };
}
