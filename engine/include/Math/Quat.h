//
// Created by pointerlost on 1/11/26.
//
#pragma once
#include "Mat4.h"

namespace Real::math {
    struct Vec3;
}

namespace Real::math {

    struct Quat {
        f32 x, y, z, w;

        Quat() noexcept : x(0), y(0), z(0), w(1) {}
        Quat(f32 x, f32 y, f32 z, f32 w) noexcept
            : x(x), y(y), z(z), w(w) {}

        static Quat Identity() noexcept { return { 0, 0, 0, 1 }; }
        Quat operator*(const Quat& r) const noexcept;

        [[nodiscard]] Quat Normalized() const noexcept;

        static Quat FromEulerRadians(const Vec3& e) noexcept;
        static Quat FromAxisAngle(const Vec3& axis, f32 radians) noexcept;
        static Quat FromEulerDegrees(const Vec3& eulerDegrees) noexcept;

        [[nodiscard]] Vec3 Rotate(const Vec3& v) const noexcept;
        [[nodiscard]] Mat4 ToMat4() const noexcept;

        [[nodiscard]] Quat Inverted()        const noexcept;
        static        Quat Inverted(const Quat& m) noexcept;
    };
}
