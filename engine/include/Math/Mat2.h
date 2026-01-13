//
// Created by pointerlost on 1/11/26.
//
#pragma once
#include "Vec2.h"
#include <glm/mat2x2.hpp>

#include "Math.h"

namespace Real::math {

    struct Mat2 {
        // Column-major: m[column][row]
        float m[2][2]{};

        constexpr Mat2(float diagonal = 1.0f) noexcept {
            for (int c = 0; c < 2; ++c)
                for (int r = 0; r < 2; ++r)
                    m[c][r] = (c == r) ? diagonal : 0.0f;
        }

        static constexpr Mat2 Identity() noexcept { return { 1.0f }; }
        Mat2 operator*(const Mat2& rhs) const noexcept;

        static Mat2 Rotate(float rad)  noexcept;
        static Mat2 Scale(const Vec2& s) noexcept;

        [[nodiscard]] const float* ValuePtr() const noexcept { return &m[0][0]; }
        [[nodiscard]] float*       ValuePtr()       noexcept { return &m[0][0]; }

        [[nodiscard]] glm::mat2 ToGLM() const noexcept;
        static Mat2 FromGLM(const glm::mat2& g) noexcept;
    };


}
