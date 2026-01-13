//
// Created by pointerlost on 1/13/26.
//
#include <Math/Mat4.h>

namespace Real::math {

    Mat4 Mat4::operator*(const Mat4 &rhs) const noexcept {
        Mat4 out(0.0f);
        for (int c = 0; c < 4; ++c)
            for (int r = 0; r < 4; ++r)
                out.m[c][r] =
                    m[0][r] * rhs.m[c][0] +
                    m[1][r] * rhs.m[c][1] +
                    m[2][r] * rhs.m[c][2] +
                    m[3][r] * rhs.m[c][3];
        return out;
    }

    Mat4 Mat4::Translate(const Vec3 &t) noexcept {
        Mat4 r(1.0f);
        r.m[3][0] = t.x;
        r.m[3][1] = t.y;
        r.m[3][2] = t.z;
        return r;
    }

    Mat4 Mat4::Scale(const Vec3 &s) noexcept {
        Mat4 r(1.0f);
        r.m[0][0] = s.x;
        r.m[1][1] = s.y;
        r.m[2][2] = s.z;
        return r;
    }

    glm::mat4 Mat4::ToGLM() const noexcept {
        glm::mat4 g(1.0f);
        for (int c = 0; c < 4; ++c)
            for (int r = 0; r < 4; ++r)
                g[c][r] = m[c][r];
        return g;
    }

    Mat4 Mat4::FromGLM(const glm::mat4 &g) noexcept {
        Mat4 r(0.0f);
        for (int c = 0; c < 4; ++c)
            for (int row = 0; row < 4; ++row)
                r.m[c][row] = g[c][row];
        return r;
    }
}
