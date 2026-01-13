//
// Created by pointerlost on 1/11/26.
//
#pragma once
#include "Vec3.h"
#include <glm/mat4x4.hpp>

#include "Vec4.h"

namespace Real::math {

    struct Mat4 {
        // Column-major: m[column][row]
        float m[4][4]{};

        constexpr Mat4(float diagonal = 1.0f) noexcept {
            for (int c = 0; c < 4; ++c)
                for (int r = 0; r < 4; ++r)
                    m[c][r] = (c == r) ? diagonal : 0.0f;
        }

        static constexpr Mat4 Identity() noexcept { return { 1.0f }; }
        Mat4 operator*(const Mat4& rhs) const noexcept;

        static Mat4 Translate(const Vec3& t) noexcept;
        static Mat4 Scale(const Vec3& s) noexcept;

        [[nodiscard]] const float* ValuePtr() const noexcept { return &m[0][0]; }
        [[nodiscard]] float*       ValuePtr()       noexcept { return &m[0][0]; }

        [[nodiscard]] glm::mat4 ToGLM() const noexcept;
        static Mat4 FromGLM(const glm::mat4& g) noexcept;
    };

}
