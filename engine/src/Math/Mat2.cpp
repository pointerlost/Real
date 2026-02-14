//
// Created by pointerlost on 1/13/26.
//
#include <Math/Mat2.h>
#include "Math/Vec2.h"

namespace Real::math {

    Mat2 Mat2::operator*(const Mat2 &rhs) const noexcept {
        Mat2 out(0.0f);
        for (int c = 0; c < 2; ++c)
            for (int r = 0; r < 2; ++r)
                out.m[c][r] =
                    m[0][r] * rhs.m[c][0] +
                    m[1][r] * rhs.m[c][1];
        return out;
    }

    Mat2 Mat2::Rotate(f32 rad) noexcept {
        const f32 c = cos(rad);
        const f32 s = sin(rad);

        Mat2 r(1.0f);
        r.m[0][0] =  c; r.m[1][0] = -s;
        r.m[0][1] =  s; r.m[1][1] =  c;
        return r;
    }

    Mat2 Mat2::Scale(const Vec2 &s) noexcept {
        Mat2 r(1.0f);
        r.m[0][0] = s.x;
        r.m[1][1] = s.y;
        return r;
    }
}
