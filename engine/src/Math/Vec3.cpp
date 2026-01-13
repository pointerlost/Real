//
// Created by pointerlost on 1/13/26.
//
#include <Math/Vec3.h>

namespace Real::math {

    Vec3 Vec3::Normalized() const noexcept {
        const float lenSq = x*x + y*y + z*z;
        if (lenSq <= 0.0f) return {};
        const float inv = 1.0f / std::sqrt(lenSq);
        return { x*inv, y*inv, z*inv };
    }
}
