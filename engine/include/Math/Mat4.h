//
// Created by pointerlost on 1/11/26.
//
#pragma once
#include "Common/RealTypes.h"

namespace Real::math {
    struct Vec3;
}

namespace Real::math {

    struct Mat4 {
        // Column-major: m[column][row]
        f32 m[4][4]{};

        constexpr Mat4(f32 diagonal = 1.0f) noexcept {
            for (int c = 0; c < 4; ++c)
                for (int r = 0; r < 4; ++r)
                    m[c][r] = (c == r) ? diagonal : 0.0f;
        }

        static constexpr Mat4 Identity() noexcept { return { 1.0f }; }
        Mat4 operator*(const Mat4& rhs) const noexcept;

        static Mat4 Translate(const Vec3& t) noexcept;
        static Mat4 Scale(const Vec3& s) noexcept;
        static Mat4 FromDirection(const Vec3& dir) noexcept;

        [[nodiscard]] const f32* ValuePtr() const noexcept { return &m[0][0]; }
        [[nodiscard]] f32*       ValuePtr()       noexcept { return &m[0][0]; }

        [[nodiscard]] Mat4 Inverted() const noexcept;
        static Mat4 Inverted(const Mat4& m) noexcept;
    };

}
