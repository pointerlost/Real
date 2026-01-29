//
// Created by pointerlost on 10/6/25.
//
#pragma once
#include "foundation/PxTransform.h"
#include "Math/Math.h"
#include "Math/Quat.h"

namespace Real {

    // NOTE: This is a WORLD transform (no hierarchy yet)
    struct Transform {
        Transform() = default;
        Transform(const Transform&) = default;
        Transform(const math::Vec3& pos, const math::Quat& quat);
        explicit Transform(const math::Vec3& pos);
        explicit Transform(const math::Quat& quat);

        // Local transform data
        math::Vec3 position{ 0.0f }; // World
        math::Quat rotation = math::Quat::Identity(); // World
        math::Vec3 scale{ 1.0f }; // World

        [[nodiscard]] math::Mat4 GetModelMatrix() const {
            // Scale and rotate in Local-Space, then move in World-Space
            // Don't change the order, changing the order can break the system
            // TRANSLATE * ROTATE * SCALE
            return  math::Mat4::Translate(position) * rotation.ToMat4() * math::Mat4::Scale(scale);
        }

        // Directions (World-space)
        // REAL engine convention: +Z is forward
        // converts local-space direction (0,0,1) (forward in object space) into world-space,
        // using the entity’s quaternion rotation.
        [[nodiscard]] math::Vec3 Forward() const { return rotation.Rotate({ 0.0f, 0.0f, 1.0f }); }
        [[nodiscard]] math::Vec3 Up()      const { return rotation.Rotate({ 0.0f, 1.0f, 0.0f }); }
        [[nodiscard]] math::Vec3 Right()   const { return rotation.Rotate({ 1.0f, 0.0f, 0.0f }); }

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

        void SetFromPhysics(const physx::PxTransform& px) {
            position = { px.p.x, px.p.y, px.p.z };
            // Don't trust external systems(like PhysX) for normalizing-ready
            rotation = math::Quat{ px.q.x, px.q.y, px.q.z, px.q.w }.Normalized();
        }
    };

}
