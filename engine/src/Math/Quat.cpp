//
// Created by pointerlost on 1/13/26.
//
#include <Math/Quat.h>

namespace Real::math {

    Quat Quat::Normalized() const noexcept {
        const float len = std::sqrt(x*x + y*y + z*z + w*w);
        if (len <= 0.0f) return Identity();
        const float inv = 1.0f / len;
        return { x*inv, y*inv, z*inv, w*inv };
    }

    Quat Quat::operator*(const Quat &r) const noexcept {
        return {
            w*r.x + x*r.w + y*r.z - z*r.y,
            w*r.y - x*r.z + y*r.w + z*r.x,
            w*r.z + x*r.y - y*r.x + z*r.w,
            w*r.w - x*r.x - y*r.y - z*r.z
        };
    }

    Quat Quat::FromEulerRadians(const Vec3 &e) noexcept {
        const float cx = std::cos(e.x * 0.5f);
        const float sx = std::sin(e.x * 0.5f);
        const float cy = std::cos(e.y * 0.5f);
        const float sy = std::sin(e.y * 0.5f);
        const float cz = std::cos(e.z * 0.5f);
        const float sz = std::sin(e.z * 0.5f);

        Quat q{};
        q.w = cy * cx * cz + sy * sx * sz;
        q.x = cy * sx * cz + sy * cx * sz;
        q.y = sy * cx * cz - cy * sx * sz;
        q.z = cy * cx * sz - sy * sx * cz;
        return q;
    }

    Quat Quat::FromAxisAngle(const Vec3 &axis, float radians) noexcept {
        const Vec3 a = axis.Normalized();
        const float s = std::sin(radians * 0.5f);
        float c = std::cos(radians * 0.5f);
        return { a.x*s, a.y*s, a.z*s, c };
    }

    Vec3 Quat::Rotate(const Vec3 &v) const noexcept {
        const Quat qv{ v.x, v.y, v.z, 0 };
        const Quat inv{ -x, -y, -z, w }; // inverse for unit quat
        Quat r = (*this) * qv * inv;
        return { r.x, r.y, r.z };
    }

    Mat4 Quat::ToMat4() const noexcept {
        const Quat q = Normalized();

        const float xx = q.x * q.x;
        const float yy = q.y * q.y;
        const float zz = q.z * q.z;
        const float xy = q.x * q.y;
        const float xz = q.x * q.z;
        const float yz = q.y * q.z;
        const float wx = q.w * q.x;
        const float wy = q.w * q.y;
        const float wz = q.w * q.z;

        Mat4 m = Mat4::Identity();
        m.m[0][0] = 1 - 2*(yy + zz);
        m.m[0][1] = 2*(xy + wz);
        m.m[0][2] = 2*(xz - wy);

        m.m[1][0] = 2*(xy - wz);
        m.m[1][1] = 1 - 2*(xx + zz);
        m.m[1][2] = 2*(yz + wx);

        m.m[2][0] = 2*(xz + wy);
        m.m[2][1] = 2*(yz - wx);
        m.m[2][2] = 1 - 2*(xx + yy);

        return m;
    }
}
