//
// Created by pointerlost on 1/11/26.
//
#pragma once
#include <cmath>
#include <glm/vec4.hpp>

#include "Vec3.h"

namespace Real::math {

    struct Vec4 {
        float x{}, y{}, z{}, w{};

        constexpr Vec4() = default;
        explicit constexpr Vec4(float v) : x(v), y(v), z(v), w(v) {}
        constexpr Vec4(const Vec3& xyz, float w) : x(xyz.x), y(xyz.y), z(xyz.z), w(w) {}
        constexpr Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

        Vec4 operator+(const Vec4& r) const noexcept { return { x+r.x, y+r.y, z+r.z, w+r.w }; }
        Vec4 operator-(const Vec4& r) const noexcept { return { x-r.x, y-r.y, z-r.z, w-r.w }; }
        Vec4 operator*(float s) const noexcept { return { x*s, y*s, z*s, w*s }; }
        Vec4 operator/(float s) const noexcept { return { x/s, y/s, z/s, w/s }; }

        Vec4& operator+=(const Vec4& r) noexcept { x+=r.x; y+=r.y; z+=r.z; w+=r.w; return *this; }
        Vec4& operator-=(const Vec4& r) noexcept { x-=r.x; y-=r.y; z-=r.z; w-=r.w; return *this; }
        Vec4& operator*=(float s) noexcept { x*=s; y*=s; z*=s; w*=s; return *this; }
        Vec4& operator/=(float s) noexcept { x/=s; y/=s; z/=s; w/=s; return *this; }

        static float Dot(const Vec4& a, const Vec4& b) noexcept { return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w; }

        [[nodiscard]] const float* ValuePtr() const noexcept { return &x; }
        float* ValuePtr() noexcept { return &x; }

        [[nodiscard]] glm::vec4 ToGLM() const   noexcept { return { x,y,z,w }; }
        static Vec4 FromGLM(const glm::vec4& v) noexcept { return { v.x,v.y,v.z,v.w }; }
    };
}
