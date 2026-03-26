//
// Created by pointerlost on 10/6/25.
//
#pragma once
#include "Math/Quat.h"
#include "Math/Vec3.h"

namespace Real {

    class Transform {
    public:
        Transform() = default;
        Transform(const Transform&) = delete;
        Transform& operator=(const Transform&) = delete;
        Transform(Transform&&) noexcept;

        explicit Transform(const math::Vec3& localPosition)
            : m_LocalPosition(localPosition) {}

        explicit Transform(const math::Quat& localRotation)
            : m_LocalRotation(localRotation.Normalized()) {}

        Transform(const math::Vec3& localPosition, const math::Quat& localRotation)
            : m_LocalPosition(localPosition),
              m_LocalRotation(localRotation.Normalized()) {}

        ~Transform() {
            DetachFromParent();
            DetachChildren();
        }

        // =====================================================
        // Hierarchy
        // =====================================================

        void SetParent(Transform* newParent) {
            if (m_Parent == newParent)
                return;

            DetachFromParent();

            m_Parent = newParent;

            if (m_Parent)
                m_Parent->m_Children.push_back(this);

            MarkDirty();
        }

        void DetachFromParent() {
            if (!m_Parent)
                return;

            std::erase(m_Parent->m_Children, this);
            m_Parent = nullptr;
        }

        void DetachChildren() {
            for (auto* child : m_Children)
                child->m_Parent = nullptr;

            m_Children.clear();
        }

        [[nodiscard]] Transform* GetParent() const { return m_Parent; }
        [[nodiscard]] const std::vector<Transform*>& GetChildren() const { return m_Children; }

        // =====================================================
        // Local Transform
        // =====================================================

        void SetLocalPosition(const math::Vec3& value) {
            m_LocalPosition = value;
            MarkDirty();
        }

        void TranslateLocal(const math::Vec3& delta) {
            m_LocalPosition += delta;
            MarkDirty();
        }

        void SetLocalRotation(const math::Quat& value) {
            m_LocalRotation = value.Normalized();
            MarkDirty();
        }

        void RotateLocal(const math::Quat& delta) {
            m_LocalRotation = (m_LocalRotation * delta).Normalized();
            MarkDirty();
        }

        void SetLocalScale(const math::Vec3& value) {
            m_LocalScale = value;
            MarkDirty();
        }

        void ScaleLocal(const math::Vec3& delta) {
            m_LocalScale += delta;
            MarkDirty();
        }

        [[nodiscard]] const math::Vec3& GetLocalPosition() const { return m_LocalPosition; }
        [[nodiscard]] const math::Quat& GetLocalRotation() const { return m_LocalRotation; }
        [[nodiscard]] const math::Vec3& GetLocalScale()    const { return m_LocalScale; }

        // =====================================================
        // World Transform
        // =====================================================

        [[nodiscard]] math::Vec3 GetWorldPosition() const {
            const auto& m = GetWorldMatrix();
            return { m[0][3], m[1][3], m[2][3] };
        }

        void SetWorldPosition(const math::Vec3& worldPos) {
            if (m_Parent) {
                m_LocalPosition =
                    m_Parent->GetWorldMatrix()
                            .Inverted()
                            .TransformPoint(worldPos);
            } else {
                m_LocalPosition = worldPos;
            }
            MarkDirty();
        }

        void TranslateWorld(const math::Vec3& delta) {
            if (m_Parent) {
                m_LocalPosition +=
                    m_Parent->GetWorldRotation()
                            .Inverted()
                            .Rotate(delta);
            } else {
                m_LocalPosition += delta;
            }
            MarkDirty();
        }

        [[nodiscard]] math::Quat GetWorldRotation() const {
            if (m_Parent)
                return (m_Parent->GetWorldRotation() * m_LocalRotation).Normalized();

            return m_LocalRotation;
        }

        void SetWorldRotation(const math::Quat& worldRot) {
            if (m_Parent) {
                m_LocalRotation =
                    (m_Parent->GetWorldRotation().Inverted() * worldRot).Normalized();
            } else {
                m_LocalRotation = worldRot.Normalized();
            }
            MarkDirty();
        }

        void RotateWorld(const math::Quat& delta) {
            SetWorldRotation((delta * GetWorldRotation()).Normalized());
        }

        [[nodiscard]] math::Vec3 GetWorldScale() const {
            const auto& m = GetWorldMatrix();

            return {
                math::Vec3{ m[0][0], m[1][0], m[2][0] }.Length(),
                math::Vec3{ m[0][1], m[1][1], m[2][1] }.Length(),
                math::Vec3{ m[0][2], m[1][2], m[2][2] }.Length()
            };
        }

        // =====================================================
        // Matrices
        // =====================================================

        [[nodiscard]] math::Mat4 GetLocalMatrix() const {
            return math::Mat4::Translate(m_LocalPosition)
                 * m_LocalRotation.ToMat4()
                 * math::Mat4::Scale(m_LocalScale);
        }

        [[nodiscard]] const math::Mat4& GetWorldMatrix() const {
            if (m_Dirty) {
                m_CachedWorldMatrix =
                    m_Parent
                    ? m_Parent->GetWorldMatrix() * GetLocalMatrix()
                    : GetLocalMatrix();

                m_Dirty = false;
            }
            return m_CachedWorldMatrix;
        }

        // =====================================================
        // Directions (World Space)
        // =====================================================

        [[nodiscard]] math::Vec3 Forward() const {
            const auto& m = GetWorldMatrix();
            return math::Vec3{ m[0][2], m[1][2], m[2][2] }.Normalized();
        }

        [[nodiscard]] math::Vec3 Up() const {
            const auto& m = GetWorldMatrix();
            return math::Vec3{ m[0][1], m[1][1], m[2][1] }.Normalized();
        }

        [[nodiscard]] math::Vec3 Right() const {
            const auto& m = GetWorldMatrix();
            return math::Vec3{ m[0][0], m[1][0], m[2][0] }.Normalized();
        }

        // =====================================================
        // Dirty state
        // =====================================================

        void MarkDirty() const {
            if (m_Dirty) // whole subtree already stale, skip
                return;

            m_Dirty = true;

            for (auto* child : m_Children)
                child->MarkDirty();
        }

    private:
        mutable math::Mat4 m_CachedWorldMatrix = math::Mat4::Identity();
        mutable bool       m_Dirty             = true;

        math::Vec3 m_LocalPosition { 0.0f };
        math::Vec3 m_LocalScale    { 1.0f };
        math::Quat m_LocalRotation = math::Quat::Identity();

        Transform*              m_Parent   = nullptr;
        std::vector<Transform*> m_Children;
    };

}
