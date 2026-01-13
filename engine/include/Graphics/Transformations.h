//
// Created by pointerlost on 10/6/25.
//
#pragma once
#include "Math/Math.h"
#include "Math/Quat.h"

namespace Real {

    struct Transform {
        // Local transform data
        math::Vec3 position{ 0.0f };
        math::Quat rotation = math::Quat::Identity();
        math::Vec3 scale{ 1.0f };

        [[nodiscard]] math::Mat4 GetModelMatrix() const {
            // TRANSLATE * ROTATE * SCALE
            return  math::Mat4::Translate(position) * rotation.ToMat4() * math::Mat4::Scale(scale);
        }

        // Directions (World-space)
        [[nodiscard]] math::Vec3 Forward() const { return rotation.Rotate({ 0.0f, 0.0f, -1.0f }); }
        [[nodiscard]] math::Vec3 Up()      const { return rotation.Rotate({ 0.0f, 1.0f,  0.0f }); }
        [[nodiscard]] math::Vec3 Right()   const { return rotation.Rotate({ 1.0f, 0.0f,  0.0f }); }

        void SetPosition(const math::Vec3& p) { position = p; }
        void Translate(const math::Vec3& delta) { position += delta; }

        // default -> rotate in object space (expected by users)
        void SetRotation(const math::Quat& q) { rotation = q.Normalized(); }
        void Rotate(const math::Quat& delta)  { rotation = (rotation * delta).Normalized(); }

        void RotateAxisAngle(const math::Vec3& axis, float radians) {
            Rotate(math::Quat::FromAxisAngle(axis, radians));
        }

        void LookAt(const math::Vec3& target, const math::Vec3& worldUp = { 0, 1, 0 }) {
            const math::Vec3 forward = (target - position).Normalized();
            rotation = math::LookRotation(forward, worldUp);
        }

        void SetScale(const math::Vec3& s) { scale = s; }

        void MultiplyScale(const math::Vec3& factor) {
            scale.x *= factor.x;
            scale.y *= factor.y;
            scale.z *= factor.z;
        }
    };

}
