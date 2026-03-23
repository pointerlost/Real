//
// Created by pointerlost on 1/11/26.
//
#pragma once
#include "Common/Types.h"

namespace Real::math {
    struct Vec3;
}

namespace Real::math {

    struct Vec4 {
        f32 x{}, y{}, z{}, w{};

        constexpr Vec4() = default;
        explicit constexpr Vec4(f32 v) : x(v), y(v), z(v), w(v) {}
        Vec4(const Vec3& xyz, f32 w);
        constexpr Vec4(f32 x, f32 y, f32 z, f32 w) : x(x), y(y), z(z), w(w) {}

        Vec4 operator+(const Vec4& r) const noexcept { return { x+r.x, y+r.y, z+r.z, w+r.w }; }
        Vec4 operator-(const Vec4& r) const noexcept { return { x-r.x, y-r.y, z-r.z, w-r.w }; }
        Vec4 operator*(f32 s) const noexcept { return { x*s, y*s, z*s, w*s }; }
        Vec4 operator/(f32 s) const noexcept { return { x/s, y/s, z/s, w/s }; }

        Vec4& operator+=(const Vec4& r) noexcept { x+=r.x; y+=r.y; z+=r.z; w+=r.w; return *this; }
        Vec4& operator-=(const Vec4& r) noexcept { x-=r.x; y-=r.y; z-=r.z; w-=r.w; return *this; }
        Vec4& operator*=(f32 s) noexcept { x*=s; y*=s; z*=s; w*=s; return *this; }
        Vec4& operator/=(f32 s) noexcept { x/=s; y/=s; z/=s; w/=s; return *this; }

        static f32 Dot(const Vec4& a, const Vec4& b) noexcept { return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w; }

        [[nodiscard]] const f32* ValuePtr() const noexcept { return &x; }
        f32* ValuePtr() noexcept { return &x; }
    };
}
