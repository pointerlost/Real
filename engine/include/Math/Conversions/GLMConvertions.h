//
// Created by pointerlost on 2/11/26.
//
#pragma once
#include <glm/ext.hpp>

namespace Real::math {
    class Mat2;
    class Mat3;
    class Mat4;
    class Quat;
    class Vec2;
    class Vec3;
    class Vec4;
}

namespace Real::interop::glm {

    [[nodiscard]] ::glm::mat2 To(const math::Mat2& m) noexcept;
    math::Mat2 From(const ::glm::mat2& g) noexcept;

    [[nodiscard]] ::glm::mat3 To(const math::Mat3& m) noexcept;
    math::Mat3 From(const ::glm::mat3& g) noexcept;

    [[nodiscard]] ::glm::mat4 To(const math::Mat4& m) noexcept;
    math::Mat4 From(const ::glm::mat4& g) noexcept;

    [[nodiscard]] ::glm::quat To(const math::Quat& q) noexcept;
    math::Quat From(const ::glm::quat& q) noexcept;

    [[nodiscard]] ::glm::vec2 To(const math::Vec2& v) noexcept;
    math::Vec2 From(const ::glm::vec2& v) noexcept;

    [[nodiscard]] ::glm::vec3 To(const math::Vec3& v) noexcept;
    math::Vec3 From(const ::glm::vec3& v) noexcept;

    [[nodiscard]] ::glm::vec4 To(const math::Vec4& v) noexcept;
    math::Vec4 From(const ::glm::vec4& v) noexcept;
}
