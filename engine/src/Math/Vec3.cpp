//
// Created by pointerlost on 1/13/26.
//
#include <Math/Vec3.h>

#include "Math/Math.h"
#include "Math/Vec2.h"

namespace Real::math {

    Vec3::Vec3(const Vec2 &xy, f32 z) : x(xy.x), y(xy.y), z(z) {}

    Vec3 Vec3::Normalized() const noexcept {
        const f32 lenSq = x*x + y*y + z*z;
        if (lenSq <= 0.0f) return {};
        const f32 inv = 1.0f / sqrt(lenSq);
        return { x*inv, y*inv, z*inv };
    }

    f32 Vec3::Length(const Vec3 &v) noexcept {
        return sqrt(LengthSq(v));
    }
}
