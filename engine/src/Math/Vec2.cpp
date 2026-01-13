//
// Created by pointerlost on 1/13/26.
//
#include <Math/Vec2.h>

namespace Real::math {

    Vec2 Vec2::Normalized() const noexcept {
        const float lenSq = x*x + y*y;
        if (lenSq <= 0.0f) return {};
        const float inv = 1.0f / std::sqrt(lenSq);
        return { x*inv, y*inv };
    }
}
