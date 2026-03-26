//
// Created by pointerlost on 10/20/25.
//
#pragma once
#include <cmath>
#include "Common/Types.h"

namespace Real::math {
    struct Quat;
    struct Mat4;
    struct Vec3;
}

namespace Real::math {
    constexpr f32 PI = 3.14159265358979323846f;

    bool DecomposeTransform(Mat4 &transform, Vec3& translation, Quat& rotation, Vec3& scale);
    int FindMax(int firstNum, int secondNum);
    [[nodiscard]] int FindClosestPowerOfTwo(int num);

    static Quat LookRotation(const Vec3& fwd, const Vec3& up) noexcept;

    [[nodiscard]] inline f32 sqrt(f32 v)      noexcept { return std::sqrt(v);      }
    [[nodiscard]] inline f32 sin(f32 radians) noexcept { return std::sin(radians); }
    [[nodiscard]] inline f32 cos(f32 radians) noexcept { return std::cos(radians); }

    [[nodiscard]] inline f32 DegreesToRadians(f32 deg) noexcept { return deg * (PI / 180.0f); }
    [[nodiscard]] inline f32 RadiansToDegrees(f32 rad) noexcept { return rad * (180.0f / PI); }

    [[nodiscard]] Vec3 DegreesToRadians(const Vec3& deg) noexcept;
    [[nodiscard]] Vec3 RadiansToDegrees(const Vec3& rad) noexcept;
    [[nodiscard]] Vec3 ToEulerDegrees(const Quat& q) noexcept;
}
