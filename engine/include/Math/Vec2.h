//
// Created by pointerlost on 1/11/26.
//
#pragma once
#include "Common/Types.h"

namespace Real::math {

    struct Vec2 {
        f32 x{}, y{};

        explicit constexpr Vec2(f32 v)        : x(v), y(v) {} // Constructor
                 constexpr Vec2(f32 x, f32 y) : x(x), y(y) {} // Constructor
                 constexpr Vec2() = default;                  // Default Constructor

        // Binary
        Vec2 operator+(const Vec2& r) const noexcept { return { x+r.x, y+r.y }; }
        Vec2 operator-(const Vec2& r) const noexcept { return { x-r.x, y-r.y }; }
        Vec2 operator*(f32 s)         const noexcept { return { x*s, y*s }; }
        Vec2 operator/(f32 s)         const noexcept { return { x/s, y/s }; }

        // Compound
        Vec2& operator+=(const Vec2& r) noexcept { x+=r.x; y+=r.y; return *this; }
        Vec2& operator-=(const Vec2& r) noexcept { x-=r.x; y-=r.y; return *this; }
        Vec2& operator*=(f32 s)         noexcept { x*=s; y*=s;     return *this; }
        Vec2& operator/=(f32 s)         noexcept { x/=s; y/=s;     return *this; }

        [[nodiscard]] Vec2 Normalized() const noexcept;

        static f32 Dot(const Vec2& a, const Vec2& b) noexcept { return a.x*b.x + a.y*b.y; }

        static f32 LengthSq(const Vec2& v) noexcept { return Dot(v,v);           }
        f32        LengthSq()        const noexcept { return Dot( {x,y},{x,y} ); }
        static f32 Length(const Vec2& v)   noexcept;
        f32        Length()          const noexcept;

        [[nodiscard]] const f32* ValuePtr() const noexcept { return &x; }
                            f32* ValuePtr()       noexcept { return &x; }
    };

}
