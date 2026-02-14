//
// Created by pointerlost on 2/11/26.
//
#include <Math/Conversions/GLMConvertions.h>
#include "Math/Mat2.h"
#include "Math/Mat3.h"
#include "Math/Mat4.h"
#include "Math/Quat.h"
#include "Math/Vec2.h"
#include "Math/Vec3.h"
#include "Math/Vec4.h"

namespace Real::interop::glm {

    ::glm::mat2 To(const math::Mat2& m) noexcept {
        ::glm::mat2 g(1.0f);
        for (int c = 0; c < 2; ++c)
            for (int r = 0; r < 2; ++r)
                g[c][r] = m.m[c][r];
        return g;
    }

    math::Mat2 From(const ::glm::mat2 &g) noexcept {
        math::Mat2 r(0.0f);
        for (int c = 0; c < 2; ++c)
            for (int row = 0; row < 2; ++row)
                r.m[c][row] = g[c][row];
        return r;
    }

    ::glm::mat3 To(const math::Mat3& m) noexcept {
        ::glm::mat3 g(1.0f);
        for (int c = 0; c < 3; ++c)
            for (int r = 0; r < 3; ++r)
                g[c][r] = m.m[c][r];
        return g;
    }

    math::Mat3 From(const ::glm::mat3 &g) noexcept {
        math::Mat3 r(0.0f);
        for (int c = 0; c < 3; ++c)
            for (int row = 0; row < 3; ++row)
                r.m[c][row] = g[c][row];
        return r;
    }

    ::glm::mat4 To(const math::Mat4& m) noexcept {
        ::glm::mat4 g(1.0f);
        for (int c = 0; c < 4; ++c)
            for (int r = 0; r < 4; ++r)
                g[c][r] = m.m[c][r];
        return g;
    }

    math::Mat4 From(const ::glm::mat4 &g) noexcept {
        math::Mat4 r(0.0f);
        for (int c = 0; c < 4; ++c)
            for (int row = 0; row < 4; ++row)
                r.m[c][row] = g[c][row];
        return r;
    }

    ::glm::quat To(const math::Quat& q) noexcept {
        return { q.w, q.x, q.y, q.z };
    }

    math::Quat From(const ::glm::quat &q) noexcept {
        return { q.x, q.y, q.z, q.w };
    }

    ::glm::vec2 To(const math::Vec2& v) noexcept {
        return { v.x, v.y };
    }

    math::Vec2 From(const ::glm::vec2 &v) noexcept {
        return { v.x,v.y };
    }

    ::glm::vec3 To(const math::Vec3 &v) noexcept {
        return { v.x, v.y, v.z };
    }

    math::Vec3 From(const ::glm::vec3 &v) noexcept {
        return { v.x,v.y,v.z };
    }

    ::glm::vec4 To(const math::Vec4 &v) noexcept {
        return { v.x, v.y, v.z, v.w };
    }

    math::Vec4 From(const ::glm::vec4 &v) noexcept {
        return { v.x,v.y,v.z,v.w };
    }
}
