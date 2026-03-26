//
// Created by pointerlost on 1/13/26.
//
#include <glm/fwd.hpp>
#include <Math/Quat.h>

#include "Math/Math.h"
#include "Math/Vec3.h"
#include "Math/Conversions/GLMConversions.h"

namespace Real::math {

    Quat Quat::Normalized() const noexcept {
        const f32 len = sqrt(x*x + y*y + z*z + w*w);
        if (len <= 0.0f) return Identity();
        const f32 inv = 1.0f / len;
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
        const f32 cx = cos(e.x * 0.5f);
        const f32 sx = sin(e.x * 0.5f);
        const f32 cy = cos(e.y * 0.5f);
        const f32 sy = sin(e.y * 0.5f);
        const f32 cz = cos(e.z * 0.5f);
        const f32 sz = sin(e.z * 0.5f);

        Quat q{};
        q.w = cy * cx * cz + sy * sx * sz;
        q.x = cy * sx * cz + sy * cx * sz;
        q.y = sy * cx * cz - cy * sx * sz;
        q.z = cy * cx * sz - sy * sx * cz;
        return q;
    }

    Quat Quat::FromAxisAngle(const Vec3 &axis, f32 radians) noexcept {
        const Vec3 a = axis.Normalized();
        const f32 s = sin(radians * 0.5f);
        f32 c = cos(radians * 0.5f);
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

        const f32 xx = q.x * q.x;
        const f32 yy = q.y * q.y;
        const f32 zz = q.z * q.z;
        const f32 xy = q.x * q.y;
        const f32 xz = q.x * q.z;
        const f32 yz = q.y * q.z;
        const f32 wx = q.w * q.x;
        const f32 wy = q.w * q.y;
        const f32 wz = q.w * q.z;

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

    Quat Quat::Inverted() const noexcept {
        // Let GLM handle the inverse
        const glm::quat inv = glm::inverse(interop::glm::To(*this));

        // Convert back to Real Mat4
        return Quat(interop::glm::From(inv));
    }

    Quat Quat::Inverted(const Quat &m) noexcept {
        // Convert Mat4 to glm::mat4
        const glm::quat glmMat = interop::glm::To(m);

        // Let GLM handle the inverse
        const glm::quat inv = glm::inverse(glmMat);

        // Convert back to Real Mat4
        return Quat(interop::glm::From(inv));
    }

    Quat Quat::FromEulerDegrees(const Vec3& eulerDegrees) noexcept {
        return FromEulerRadians(DegreesToRadians(eulerDegrees));
    }
}
