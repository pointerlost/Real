//
// Created by pointerlost on 1/11/26.
//
#pragma once
#include <cmath>
#include <glm/vec2.hpp>

namespace Real::math {

    struct Vec2 {
        float x{}, y{};

        constexpr Vec2() = default;
        explicit constexpr Vec2(float v) : x(v), y(v) {}
        constexpr Vec2(float x, float y) : x(x), y(y) {}

        // Binary
        Vec2 operator+(const Vec2& r) const noexcept { return { x+r.x, y+r.y }; }
        Vec2 operator-(const Vec2& r) const noexcept { return { x-r.x, y-r.y }; }
        Vec2 operator*(float s) const noexcept { return { x*s, y*s }; }
        Vec2 operator/(float s) const noexcept { return { x/s, y/s }; }

        // Compound
        Vec2& operator+=(const Vec2& r) noexcept { x+=r.x; y+=r.y; return *this; }
        Vec2& operator-=(const Vec2& r) noexcept { x-=r.x; y-=r.y; return *this; }
        Vec2& operator*=(float s) noexcept { x*=s; y*=s; return *this; }
        Vec2& operator/=(float s) noexcept { x/=s; y/=s; return *this; }

        [[nodiscard]] Vec2 Normalized() const noexcept;

        static float Dot(const Vec2& a, const Vec2& b) noexcept { return a.x*b.x + a.y*b.y; }
        static float LengthSq(const Vec2& v) noexcept { return Dot(v,v); }
        static float Length(const Vec2& v)   noexcept { return std::sqrt(LengthSq(v)); }

        [[nodiscard]] const float* ValuePtr() const noexcept { return &x; }
        float* ValuePtr() noexcept { return &x; }

        [[nodiscard]] glm::vec2 ToGLM() const noexcept { return { x,y }; }
        static Vec2 FromGLM(const glm::vec2& v) noexcept { return { v.x,v.y }; }
    };

}
