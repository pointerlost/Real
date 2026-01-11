//
// Created by pointerlost on 1/11/26.
//
#pragma once
#include "Mat4.h"

namespace Real::Math {

    struct Mat3 {
        float m[3][3]{};

        Mat3() noexcept {
            *this = Identity();
        }

        static Mat3 Identity() noexcept {
            Mat3 r{};
            r.m[0][0] = 1.0f;
            r.m[1][1] = 1.0f;
            r.m[2][2] = 1.0f;
            return r;
        }

        static Mat3 Scale(const Vec2& s) noexcept {
            Mat3 r = Identity();
            r.m[0][0] = s.x;
            r.m[1][1] = s.y;
            return r;
        }

        static Mat3 RotationZ(float rad) noexcept {
            Mat3 r = Identity();
            float c = std::cos(rad);
            float s = std::sin(rad);

            r.m[0][0] =  c; r.m[1][0] = -s;
            r.m[0][1] =  s; r.m[1][1] =  c;
            return r;
        }

        Vec3 operator*(const Vec3& v) const noexcept {
            return {
                m[0][0]*v.x + m[1][0]*v.y + m[2][0]*v.z,
                m[0][1]*v.x + m[1][1]*v.y + m[2][1]*v.z,
                m[0][2]*v.x + m[1][2]*v.y + m[2][2]*v.z
            };
        }

        Mat3 operator*(const Mat3& r) const noexcept {
            Mat3 out{};
            for (int c = 0; c < 3; ++c)
                for (int row = 0; row < 3; ++row)
                    out.m[c][row] =
                        m[0][row] * r.m[c][0] +
                        m[1][row] * r.m[c][1] +
                        m[2][row] * r.m[c][2];
            return out;
        }
    };

}
