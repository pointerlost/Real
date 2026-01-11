//
// Created by pointerlost on 1/11/26.
//
#pragma once
#include <cmath>
#include <glm/vec2.hpp>

namespace Real::Math {

    struct Vec2 {
        float x, y;

        Vec2() = default;
        Vec2(float x, float y) : x(x), y(y) {}

        Vec2 operator+(const Vec2& r) const noexcept {
            return { x + r.x, y + r.y };
        }

        Vec2 operator-(const Vec2& r) const noexcept {
            return { x - r.x, y - r.y };
        }

        Vec2 operator*(float s) const noexcept {
            return { x * s, y * s };
        }

        friend Vec2 operator*(float s, const Vec2& v) noexcept {
            return { v.x*s, v.y*s };
        }

        [[nodiscard]] glm::vec2 ToGLM() const noexcept { return {x, y}; }
        static Vec2 FromGLM(const glm::vec2& v) {
            return { v.x, v.y };
        }

        // The original vector doesn't change, it's just returning a new one
        [[nodiscard]] Vec2 Normalized() const noexcept {
            // Compute "Squared Length" -> square of |v|
            const float lenSq = x*x + y*y;
            if (lenSq <= 0.0f) return { 0,0 }; // Zero check (can't division by zero) and Null values like 0.0f
            const float inv = 1.0f / std::sqrt(lenSq); // Computes 1 / |v|
            // Scale vector
            return { x*inv, y*inv };
        }

        [[nodiscard]] static float Dot(const Vec2& a, const Vec2& b) noexcept {
            return a.x*b.x + a.y*b.y;
        }

        [[nodiscard]] static float Length(const Vec2& v) noexcept {
            return std::sqrt(LengthSq(v));
        }

        [[nodiscard]] static float LengthSq(const Vec2& v) noexcept {
            return Dot(v, v);
        }
    };
}
