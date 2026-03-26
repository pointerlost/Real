//
// Created by pointerlost on 2/23/26.
//
#pragma once
#include "Common/Types.h"

namespace Real::math {

    struct iVec2 {
        i32 x{}, y{};

        constexpr iVec2() = default;
        explicit constexpr iVec2(i32 v) : x(v), y(v) {}
        constexpr iVec2(i32 x, i32 y) : x(x), y(y) {}

        // Binary
        iVec2 operator+(const iVec2& r) const noexcept { return { x+r.x, y+r.y }; }
        iVec2 operator-(const iVec2& r) const noexcept { return { x-r.x, y-r.y }; }
        iVec2 operator*(i32 s) const noexcept { return { x*s, y*s }; }
        iVec2 operator/(i32 s) const noexcept { return { x/s, y/s }; }

        // Compound
        iVec2& operator+=(const iVec2& r) noexcept { x+=r.x; y+=r.y; return *this; }
        iVec2& operator-=(const iVec2& r) noexcept { x-=r.x; y-=r.y; return *this; }
        iVec2& operator*=(i32 s) noexcept { x*=s; y*=s; return *this; }
        iVec2& operator/=(i32 s) noexcept { x/=s; y/=s; return *this; }

        static i32 Dot(const iVec2& a, const iVec2& b) noexcept { return a.x*b.x + a.y*b.y; }

        static i32 LengthSq(const iVec2& v) noexcept { return Dot(v,v); }
        i32        LengthSq()         const noexcept { return Dot( {x,y},{x,y} ); }
        static i32 Length(const iVec2& v)   noexcept;
        i32        Length()           const noexcept;

        [[nodiscard]] const i32* ValuePtr() const noexcept { return &x; }
        i32*                     ValuePtr()       noexcept { return &x; }
    };

}
