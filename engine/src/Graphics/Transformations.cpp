//
// Created by pointerlost on 10/9/25.
//
#include "Graphics/Transformations.h"

namespace Real {

    Transform::Transform(const math::Vec3 &pos, const math::Quat &quat, const math::Vec3& scale)
        : position(pos), rotation(quat), scale(scale)
    {
    }

    Transform::Transform(const math::Vec3 &pos) : position(pos)
    {
    }

    Transform::Transform(const math::Quat &quat) : rotation(quat)
    {
    }
}
