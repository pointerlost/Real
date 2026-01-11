//
// Created by pointerlost on 1/11/26.
//
#pragma once
#include <cmath>
#include <glm/vec3.hpp>

namespace Real::Math {

    struct Vec3 {
        float x, y, z;

        Vec3() = default;
        Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

        Vec3 operator+(const Vec3& r) const noexcept {
            return { x + r.x, y + r.y, z + r.z };
        }

        Vec3 operator-(const Vec3& r) const noexcept {
            return { x - r.x, y - r.y, z - r.z };
        }

        Vec3 operator*(float s) const noexcept {
            return { x * s, y * s, z * s };
        }

        friend Vec3 operator*(float s, const Vec3& v) noexcept {
            return { v.x*s, v.y*s, v.z*s };
        }

        [[nodiscard]] glm::vec3 ToGLM() const noexcept { return {x, y, z}; }
        static Vec3 FromGLM(const glm::vec3& v) {
            return { v.x, v.y, v.z };
        }

        // The original vector doesn't change, it's just returning a new one
        [[nodiscard]] Vec3 Normalized() const noexcept {
            // Compute "Squared Length" -> square of |v|
            const float lenSq = x*x + y*y + z*z;
            if (lenSq <= 0.0f) return { 0, 0, 0 }; // Zero check (can't division by zero) and Null values like 0.0f
            const float inv = 1.0f / std::sqrt(lenSq); // Computes 1 / |v|
            // Scale vector
            return { x*inv, y*inv, z*inv };
        }

        [[nodiscard]] static float Dot(const Vec3& a, const Vec3& b) noexcept {
            return a.x*b.x + a.y*b.y + a.z*b.z;
        }

        [[nodiscard]] static float Length(const Vec3& v) noexcept {
            return std::sqrt(LengthSq(v));
        }

        [[nodiscard]] static float LengthSq(const Vec3& v) noexcept {
            return Dot(v, v);
        }
    };
}
