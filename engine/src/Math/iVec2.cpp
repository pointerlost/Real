//
// Created by pointerlost on 2/23/26.
//
#include <Math/iVec2.h>
#include "Math/Math.h"

namespace Real::math {

    i32 iVec2::Length(const iVec2& v) noexcept {
        return static_cast<i32>(sqrtf(static_cast<float>(v.x*v.x + v.y*v.y)));
    }
}
