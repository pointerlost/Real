//
// Created by pointerlost on 1/11/26.
//
#pragma once
#include <cmath>
#include <glm/vec3.hpp>

#include "Vec2.h"

namespace Real::math {

    struct Vec3 {
        float x{}, y{}, z{};

        constexpr Vec3() = default;
        explicit constexpr Vec3(float v) : x(v), y(v), z(v) {}
        constexpr Vec3(const Vec2& xy, float z) : x(xy.x), y(xy.y), z(z) {}
        constexpr Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

        // Binary
        Vec3 operator+(const Vec3& r) const noexcept { return { x+r.x, y+r.y, z+r.z }; }
        Vec3 operator-(const Vec3& r) const noexcept { return { x-r.x, y-r.y, z-r.z }; }
        Vec3 operator*(float s) const noexcept { return { x*s, y*s, z*s }; }
        Vec3 operator/(float s) const noexcept { return { x/s, y/s, z/s }; }

        // Compound
        Vec3& operator+=(const Vec3& r) noexcept { x+=r.x; y+=r.y; z+=r.z; return *this; }
        Vec3& operator-=(const Vec3& r) noexcept { x-=r.x; y-=r.y; z-=r.z; return *this; }
        Vec3& operator*=(float s) noexcept { x*=s; y*=s; z*=s; return *this; }
        Vec3& operator/=(float s) noexcept { x/=s; y/=s; z/=s; return *this; }

        [[nodiscard]] Vec3 Normalized() const noexcept;

        static float Dot(const Vec3& a, const Vec3& b) noexcept { return a.x*b.x + a.y*b.y + a.z*b.z; }
        [[nodiscard]] Vec3 Cross(const Vec3& r) const noexcept {
            return {
                y*r.z - z*r.y,
                z*r.x - x*r.z,
                x*r.y - y*r.x
            };
        }

        static float LengthSq(const Vec3& v) noexcept { return Dot(v,v); }
        static float Length(const Vec3& v) noexcept { return std::sqrt(LengthSq(v)); }

        [[nodiscard]] const float* ValuePtr() const noexcept { return &x; }
        float* ValuePtr() noexcept { return &x; }

        [[nodiscard]] glm::vec3 ToGLM() const noexcept { return { x,y,z }; }
        static Vec3 FromGLM(const glm::vec3& v) noexcept { return { v.x,v.y,v.z }; }
    };

}
