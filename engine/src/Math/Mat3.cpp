//
// Created by pointerlost on 1/13/26.
//
#include <Math/Mat3.h>

namespace Real::math {

    Mat3 Mat3::operator*(const Mat3 &rhs) const noexcept {
        Mat3 out(0.0f);
        for (int c = 0; c < 3; ++c)
            for (int r = 0; r < 3; ++r)
                out.m[c][r] =
                    m[0][r] * rhs.m[c][0] +
                    m[1][r] * rhs.m[c][1] +
                    m[2][r] * rhs.m[c][2];
        return out;
    }

    Mat3 Mat3::Translate(const Vec2 &t) noexcept {
        Mat3 r(1.0f);
        r.m[2][0] = t.x;
        r.m[2][1] = t.y;
        return r;
    }

    Mat3 Mat3::Rotate(float rad) noexcept {
        const float c = std::cos(rad);
        const float s = std::sin(rad);

        Mat3 r(1.0f);
        r.m[0][0] =  c; r.m[1][0] = -s;
        r.m[0][1] =  s; r.m[1][1] =  c;
        return r;
    }

    Mat3 Mat3::Scale(const Vec2 &s) noexcept {
        Mat3 r(1.0f);
        r.m[0][0] = s.x;
        r.m[1][1] = s.y;
        return r;
    }

    glm::mat3 Mat3::ToGLM() const noexcept {
        glm::mat3 g(1.0f);
        for (int c = 0; c < 3; ++c)
            for (int r = 0; r < 3; ++r)
                g[c][r] = m[c][r];
        return g;
    }

    Mat3 Mat3::FromGLM(const glm::mat3 &g) noexcept {
        Mat3 r(0.0f);
        for (int c = 0; c < 3; ++c)
            for (int row = 0; row < 3; ++row)
                r.m[c][row] = g[c][row];
        return r;
    }
}
