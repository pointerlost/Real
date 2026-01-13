//
// Created by pointerlost on 1/11/26.
//
#pragma once
#include "Mat4.h"
#include "Vec3.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Real::math {

    struct Quat {
        float x, y, z, w;

        Quat() noexcept : x(0), y(0), z(0), w(1) {}
        Quat(float x, float y, float z, float w) noexcept
            : x(x), y(y), z(z), w(w) {}

        static Quat Identity() noexcept { return { 0, 0, 0, 1 }; }
        Quat operator*(const Quat& r) const noexcept;

        [[nodiscard]] Quat Normalized() const noexcept;

        static Quat FromEulerRadians(const Vec3& e) noexcept;
        static Quat FromAxisAngle(const Vec3& axis, float radians) noexcept;
        [[nodiscard]] Vec3 Rotate(const Vec3& v) const noexcept;

        [[nodiscard]] Mat4 ToMat4() const noexcept;
        [[nodiscard]] glm::quat ToGLM() const noexcept { return {w, x, y, z}; }
        static Quat FromGLM(const glm::quat& q) noexcept { return { q.x, q.y, q.z, q.w }; }
    };
}
