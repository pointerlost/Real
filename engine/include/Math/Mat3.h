//
// Created by pointerlost on 1/11/26.
//
#pragma once
#include "Common/RealTypes.h"

namespace Real::math {
    struct Vec2;
}

namespace Real::math {

    struct Mat3 {
        // Column-major: m[column][row]
        f32 m[3][3]{};

        constexpr Mat3(f32 diagonal = 1.0f) noexcept {
            for (int c = 0; c < 3; ++c)
                for (int r = 0; r < 3; ++r)
                    m[c][r] = (c == r) ? diagonal : 0.0f;
        }

        static constexpr Mat3 Identity() noexcept { return { 1.0f }; }
        Mat3 operator*(const Mat3& rhs) const noexcept;

        static Mat3 Translate(const Vec2& t) noexcept;
        static Mat3 Rotate(f32 rad) noexcept;
        static Mat3 Scale(const Vec2& s) noexcept;

        [[nodiscard]] const f32* ValuePtr() const noexcept { return &m[0][0]; }
        [[nodiscard]] f32*       ValuePtr()       noexcept { return &m[0][0]; }
    };

}
