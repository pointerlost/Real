//
// Created by pointerlost on 1/11/26.
//
#pragma once
#include <cmath>
#include "Vec2.h"

namespace Real::Math {

    struct Mat2 {
        // column-major: m[col][row]
        float m[2][2]{};

        Mat2() noexcept {
            *this = Identity();
        }

        static Mat2 Identity() noexcept {
            Mat2 r{};
            r.m[0][0] = 1.0f;
            r.m[1][1] = 1.0f;
            return r;
        }

        static Mat2 Rotation(float rad) noexcept {
            Mat2 r{};
            float c = std::cos(rad);
            float s = std::sin(rad);

            r.m[0][0] =  c; r.m[1][0] = -s;
            r.m[0][1] =  s; r.m[1][1] =  c;
            return r;
        }

        Vec2 operator*(const Vec2& v) const noexcept {
            return {
                m[0][0] * v.x + m[1][0] * v.y,
                m[0][1] * v.x + m[1][1] * v.y
            };
        }

        Mat2 operator*(const Mat2& r) const noexcept {
            Mat2 out{};
            for (int c = 0; c < 2; ++c)
                for (int row = 0; row < 2; ++row)
                    out.m[c][row] =
                        m[0][row] * r.m[c][0] +
                        m[1][row] * r.m[c][1];
            return out;
        }
    };

}
