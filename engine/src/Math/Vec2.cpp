//
// Created by pointerlost on 1/13/26.
//
#include <Math/Vec2.h>
#include "Math/Math.h"

namespace Real::math {

    Vec2 Vec2::Normalized() const noexcept {
        const f32 lenSq = x*x + y*y;
        if (lenSq <= 0.0f) return {};
        const f32 inv = 1.0f / sqrt(lenSq);
        return { x*inv, y*inv };
    }

    f32 Vec2::Length(const Vec2 &v) noexcept {
        return sqrt(LengthSq(v));
    }
}
