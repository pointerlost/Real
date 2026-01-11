//
// Created by pointerlost on 1/11/26.
//
#pragma once
#include <cmath>
#include <glm/vec4.hpp>

namespace Real::Math {

    struct Vec4 {
        float x, y, z, w;

        Vec4() = default;
        Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

        Vec4 operator+(const Vec4& r) const noexcept {
            return { x + r.x, y + r.y, z + r.z, w + r.w };
        }

        Vec4 operator-(const Vec4& r) const noexcept {
            return { x - r.x, y - r.y, z - r.z, w - r.w };
        }

        Vec4 operator*(float s) const noexcept {
            return { x * s, y * s, z * s, w * s };
        }

        friend Vec4 operator*(float s, const Vec4& v) noexcept {
            return { v.x*s, v.y*s, v.z*s, v.w*s };
        }

        [[nodiscard]] glm::vec4 ToGLM() const noexcept { return {x, y, z, w}; }
        static Vec4 FromGLM(const glm::vec4& v) {
            return { v.x, v.y, v.z, v.w };
        }

        // The original vector doesn't change, it's just returning a new one
        [[nodiscard]] Vec4 Normalized() const noexcept {
            // Compute "Squared Length" -> so square of |v|
            const float lenSq = x*x + y*y + z*z + w*w;
            if (lenSq <= 0.0f) return { 0, 0, 0, 0 };; // Zero check (can't division by zero) and Null values like 0.0f
            const float inv = 1.0f / std::sqrt(lenSq); // Computes 1 / |v|
            // Scale vector
            return { x*inv, y*inv, z*inv, w*inv };
        }

        [[nodiscard]] static float Dot(const Vec4& a, const Vec4& b) noexcept {
            return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
        }

        [[nodiscard]] static float Length(const Vec4& v) noexcept {
            return std::sqrt(LengthSq(v));
        }

        [[nodiscard]] static float LengthSq(const Vec4& v) noexcept {
            return Dot(v, v);
        }
    };
}
