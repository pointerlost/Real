//
// Created by pointerlost on 1/13/26.
//
#include <glm/gtc/type_ptr.hpp>
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

    Mat4 Mat4::FromDirection(const Vec3 &dir) noexcept {
        const Vec3 forward = dir.Normalized();

        // Fallback if direction is invalid
        if (Vec3::LengthSq(forward) < 1e-6f) {
            return Identity();
        }

        // Choose a stable up vector
        const Vec3 worldUp = std::abs(forward.y) > 0.99f ? Vec3{1, 0, 0} : Vec3{0, 1, 0};

        const Vec3 right = worldUp.Cross(forward).Normalized();
        const Vec3 up    = forward.Cross(right);

        Mat4 result(1.0f);

        // Column-major layout
        result.m[0][0] = forward.x;
        result.m[0][1] = forward.y;
        result.m[0][2] = forward.z;

        result.m[1][0] = up.x;
        result.m[1][1] = up.y;
        result.m[1][2] = up.z;

        result.m[2][0] = right.x;
        result.m[2][1] = right.y;
        result.m[2][2] = right.z;

        return result;
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

    Mat4 Mat4::Inverted() const noexcept {
        // Let GLM handle the inverse
        const glm::mat4 inv = glm::inverse(ToGLM());

        // Convert back to Real Mat4
        return Mat4(FromGLM(inv));
    }

    Mat4 Mat4::Inverted(const Mat4 &m) noexcept {
        // Convert Mat4 to glm::mat4
        const glm::mat4 glmMat = m.ToGLM();

        // Let GLM handle the inverse
        const glm::mat4 inv = glm::inverse(glmMat);

        // Convert back to Real Mat4
        return Mat4(FromGLM(inv));
    }
}
