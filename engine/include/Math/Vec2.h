//
// Created by pointerlost on 1/11/26.
//
#pragma once
#include "Common/RealTypes.h"

namespace Real::math {

    struct Vec2 {
        f32 x{}, y{};

        constexpr Vec2() = default;
        explicit constexpr Vec2(f32 v) : x(v), y(v) {}
        constexpr Vec2(f32 x, f32 y) : x(x), y(y) {}

        // Binary
        Vec2 operator+(const Vec2& r) const noexcept { return { x+r.x, y+r.y }; }
        Vec2 operator-(const Vec2& r) const noexcept { return { x-r.x, y-r.y }; }
        Vec2 operator*(f32 s) const noexcept { return { x*s, y*s }; }
        Vec2 operator/(f32 s) const noexcept { return { x/s, y/s }; }

        // Compound
        Vec2& operator+=(const Vec2& r) noexcept { x+=r.x; y+=r.y; return *this; }
        Vec2& operator-=(const Vec2& r) noexcept { x-=r.x; y-=r.y; return *this; }
        Vec2& operator*=(f32 s) noexcept { x*=s; y*=s; return *this; }
        Vec2& operator/=(f32 s) noexcept { x/=s; y/=s; return *this; }

        [[nodiscard]] Vec2 Normalized() const noexcept;

        static f32 Dot(const Vec2& a, const Vec2& b) noexcept { return a.x*b.x + a.y*b.y; }
        static f32 LengthSq(const Vec2& v) noexcept { return Dot(v,v); }
        static f32 Length(const Vec2& v)   noexcept;

        [[nodiscard]] const f32* ValuePtr() const noexcept { return &x; }
        f32* ValuePtr() noexcept { return &x; }
    };

}
