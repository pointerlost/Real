//
// Created by pointerlost on 1/11/26.
//
#pragma once
#include "Vec3.h"
#include <glm/mat4x4.hpp>

namespace Real::Math {

    struct Mat4 {
        float m[4][4]{};

        Mat4() noexcept {
            *this = Identity();
        }

        static Mat4 Identity() noexcept {
            Mat4 r{};
            r.m[0][0] = 1.0f;
            r.m[1][1] = 1.0f;
            r.m[2][2] = 1.0f;
            r.m[3][3] = 1.0f;
            return r;
        }

        static Mat4 Translate(const Vec3& t) noexcept {
            Mat4 r = Identity();
            r.m[3][0] = t.x;
            r.m[3][1] = t.y;
            r.m[3][2] = t.z;
            return r;
        }

        static Mat4 Scale(const Vec3& s) noexcept {
            Mat4 r = Identity();
            r.m[0][0] = s.x;
            r.m[1][1] = s.y;
            r.m[2][2] = s.z;
            return r;
        }

        static Mat4 RotateAxisAngle(const Vec3& axis, float rad) noexcept {
            const Vec3 a = axis.Normalized();
            const float c = std::cos(rad);
            const float s = std::sin(rad);
            const float t = 1.0f - c;

            Mat4 r = Identity();

            r.m[0][0] = c + a.x*a.x*t;
            r.m[0][1] = a.x*a.y*t + a.z*s;
            r.m[0][2] = a.x*a.z*t - a.y*s;

            r.m[1][0] = a.y*a.x*t - a.z*s;
            r.m[1][1] = c + a.y*a.y*t;
            r.m[1][2] = a.y*a.z*t + a.x*s;

            r.m[2][0] = a.z*a.x*t + a.y*s;
            r.m[2][1] = a.z*a.y*t - a.x*s;
            r.m[2][2] = c + a.z*a.z*t;

            return r;
        }

        Mat4 operator*(const Mat4& r) const noexcept {
            Mat4 out{};
            for (int c = 0; c < 4; ++c)
                for (int row = 0; row < 4; ++row)
                    out.m[c][row] =
                        m[0][row] * r.m[c][0] +
                        m[1][row] * r.m[c][1] +
                        m[2][row] * r.m[c][2] +
                        m[3][row] * r.m[c][3];
            return out;
        }

        [[nodiscard]] Vec3 TransformPoint(const Vec3& v) const noexcept {
            return {
                m[0][0]*v.x + m[1][0]*v.y + m[2][0]*v.z + m[3][0],
                m[0][1]*v.x + m[1][1]*v.y + m[2][1]*v.z + m[3][1],
                m[0][2]*v.x + m[1][2]*v.y + m[2][2]*v.z + m[3][2]
            };
        }
    };

}
