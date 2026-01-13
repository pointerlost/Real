//
// Created by pointerlost on 1/13/26.
//
#include <Math/Mat2.h>

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

    Mat2 Mat2::Rotate(float rad) noexcept {
        const float c = cos(rad);
        const float s = sin(rad);

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

    glm::mat2 Mat2::ToGLM() const noexcept {
        glm::mat2 g(1.0f);
        for (int c = 0; c < 2; ++c)
            for (int r = 0; r < 2; ++r)
                g[c][r] = m[c][r];
        return g;
    }

    Mat2 Mat2::FromGLM(const glm::mat2 &g) noexcept {
        Mat2 r(0.0f);
        for (int c = 0; c < 2; ++c)
            for (int row = 0; row < 2; ++row)
                r.m[c][row] = g[c][row];
        return r;
    }
}
