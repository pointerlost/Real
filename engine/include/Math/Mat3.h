//
// Created by pointerlost on 1/11/26.
//
#pragma once
#include "Mat4.h"
#include "Vec2.h"

namespace Real::math {

    struct Mat3 {
        // Column-major: m[column][row]
        float m[3][3]{};

        constexpr Mat3(float diagonal = 1.0f) noexcept {
            for (int c = 0; c < 3; ++c)
                for (int r = 0; r < 3; ++r)
                    m[c][r] = (c == r) ? diagonal : 0.0f;
        }

        static constexpr Mat3 Identity() noexcept { return { 1.0f }; }
        Mat3 operator*(const Mat3& rhs) const noexcept;

        static Mat3 Translate(const Vec2& t) noexcept;
        static Mat3 Rotate(float rad) noexcept;
        static Mat3 Scale(const Vec2& s) noexcept;

        [[nodiscard]] const float* ValuePtr() const noexcept { return &m[0][0]; }
        [[nodiscard]] float*       ValuePtr()       noexcept { return &m[0][0]; }

        [[nodiscard]] glm::mat3 ToGLM() const noexcept;
        static Mat3 FromGLM(const glm::mat3& g) noexcept;
    };

}
