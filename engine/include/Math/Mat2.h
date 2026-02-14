//
// Created by pointerlost on 1/11/26.
//
#pragma once
#include "Math.h"

namespace Real::math {
    struct Vec2;
}

namespace Real::math {

    struct Mat2 {
        // Column-major: m[column][row]
        f32 m[2][2]{};

        constexpr Mat2(f32 diagonal = 1.0f) noexcept {
            for (int c = 0; c < 2; ++c)
                for (int r = 0; r < 2; ++r)
                    m[c][r] = (c == r) ? diagonal : 0.0f;
        }

        static constexpr Mat2 Identity() noexcept { return { 1.0f }; }
        Mat2 operator*(const Mat2& rhs) const noexcept;

        static Mat2 Rotate(f32 rad)  noexcept;
        static Mat2 Scale(const Vec2& s) noexcept;

        [[nodiscard]] const f32* ValuePtr() const noexcept { return &m[0][0]; }
        [[nodiscard]] f32*       ValuePtr()       noexcept { return &m[0][0]; }
    };


}
