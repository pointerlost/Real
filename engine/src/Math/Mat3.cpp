//
// Created by pointerlost on 1/13/26.
//
#include <Math/Mat3.h>

#include "Math/Math.h"
#include "Math/Vec2.h"

namespace Real::math {

    Mat3 Mat3::operator*(const Mat3 &rhs) const noexcept {
        Mat3 out(0.0f);
        for (int c = 0; c < 3; ++c)
            for (int r = 0; r < 3; ++r)
                out.m[c][r] =
                    m[0][r] * rhs.m[c][0] +
                    m[1][r] * rhs.m[c][1] +
                    m[2][r] * rhs.m[c][2];
        return out;
    }

    Mat3 Mat3::Translate(const Vec2 &t) noexcept {
        Mat3 r(1.0f);
        r.m[2][0] = t.x;
        r.m[2][1] = t.y;
        return r;
    }

    Mat3 Mat3::Rotate(f32 rad) noexcept {
        const f32 c = cos(rad);
        const f32 s = sin(rad);

        Mat3 r(1.0f);
        r.m[0][0] =  c; r.m[1][0] = -s;
        r.m[0][1] =  s; r.m[1][1] =  c;
        return r;
    }

    Mat3 Mat3::Scale(const Vec2 &s) noexcept {
        Mat3 r(1.0f);
        r.m[0][0] = s.x;
        r.m[1][1] = s.y;
        return r;
    }
}
