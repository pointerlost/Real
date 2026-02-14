//
// Created by pointerlost on 1/13/26.
//
#include <Math/Vec4.h>
#include "Math/Vec3.h"

namespace Real::math {
    Vec4::Vec4(const Vec3 &xyz, f32 w) : x(xyz.x), y(xyz.y), z(xyz.z), w(w) {}
}
