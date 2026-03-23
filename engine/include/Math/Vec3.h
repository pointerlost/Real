//
// Created by pointerlost on 1/11/26.
//
#pragma once
#include "Common/Types.h"

namespace Real::math {
    struct Vec2;
}

namespace Real::math {

    struct Vec3 {
        f32 x{}, y{}, z{};

        constexpr Vec3() = default;
        explicit constexpr Vec3(f32 v) : x(v), y(v), z(v) {}
        Vec3(const Vec2& xy, f32 z);
        constexpr Vec3(f32 x, f32 y, f32 z) : x(x), y(y), z(z) {}

        // Binary
        Vec3 operator+(const Vec3& r) const noexcept { return { x+r.x, y+r.y, z+r.z }; }
        Vec3 operator-(const Vec3& r) const noexcept { return { x-r.x, y-r.y, z-r.z }; }
        Vec3 operator*(f32 s) const noexcept { return { x*s, y*s, z*s }; }
        Vec3 operator/(f32 s) const noexcept { return { x/s, y/s, z/s }; }

        // Compound
        Vec3& operator+=(const Vec3& r) noexcept { x+=r.x; y+=r.y; z+=r.z; return *this; }
        Vec3& operator-=(const Vec3& r) noexcept { x-=r.x; y-=r.y; z-=r.z; return *this; }
        Vec3& operator*=(f32 s) noexcept { x*=s; y*=s; z*=s; return *this; }
        Vec3& operator/=(f32 s) noexcept { x/=s; y/=s; z/=s; return *this; }

        [[nodiscard]] Vec3 Normalized() const noexcept;

        static f32 Dot(const Vec3& a, const Vec3& b) noexcept { return a.x*b.x + a.y*b.y + a.z*b.z; }
        [[nodiscard]] Vec3 Cross(const Vec3& r) const noexcept {
            return {
                y*r.z - z*r.y,
                z*r.x - x*r.z,
                x*r.y - y*r.x
            };
        }

        static f32 LengthSq(const Vec3& v) noexcept { return Dot(v,v); }
        static f32 Length(const Vec3& v) noexcept;

        [[nodiscard]] const f32* ValuePtr() const noexcept { return &x; }
        f32* ValuePtr() noexcept { return &x; }
    };

}
