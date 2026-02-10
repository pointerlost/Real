//
// Created by pointerlost on 10/20/25.
//
#include "Math/Math.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/matrix_decompose.hpp"
#include "Math/Mat4.h"
#include "Math/Quat.h"

namespace Real::math {

    bool DecomposeTransform(Mat4& transform, Vec3& translation, Quat& rotation, Vec3& scale) {
        const glm::mat4 glmTransform = transform.ToGLM();
        glm::vec3 glmTranslation     = {};
        glm::quat glmRotation        = {};
        glm::vec3 glmScale           = {};

        // From glm::decompose in matrix_decompose.inl
        using namespace glm;
        using T = float;

        mat4 LocalMatrix(glmTransform);

        // Normalize the matrix.
        if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))
            return false;

        // First, isolate perspective.  This is the messiest.
        if (
            epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
            epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
            epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>()))
        {
            // Clear the perspective partition
            LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
            LocalMatrix[3][3] = static_cast<T>(1);
        }

        // Next take care of translation (easy).
        glmTranslation = vec3(LocalMatrix[3]);
        LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

        vec3 Row[3], Pdum3;

        // Now get scale and shear.
        for (length_t i = 0; i < 3; ++i)
            for (length_t j = 0; j < 3; ++j)
                Row[i][j] = LocalMatrix[i][j];

        // Compute X scale factor and normalize first row.
        glmScale.x = length(Row[0]);
        Row[0] = detail::scale(Row[0], static_cast<T>(1));
        glmScale.y = length(Row[1]);
        Row[1] = detail::scale(Row[1], static_cast<T>(1));
        glmScale.z = length(Row[2]);
        Row[2] = detail::scale(Row[2], static_cast<T>(1));

        // At this point, the matrix (in rows[]) is orthonormal.
        // Check for a coordinate system flip.  If the determinant
        // is -1, then negate the matrix and the scaling factors.
    #if 0
        Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
        if (dot(Row[0], Pdum3) < 0)
        {
            for (length_t i = 0; i < 3; i++)
            {
                scale[i] *= static_cast<T>(-1);
                Row[i] *= static_cast<T>(-1);
            }
        }
    #endif

        // Convert the 3x3 rotation matrix to quaternion
        const auto rotationMatrix = mat3(Row[0], Row[1], Row[2]);
        glmRotation = quat_cast(rotationMatrix);

        // Convert glm math to REAL custom math
        transform   = Mat4::FromGLM(glmTransform);
        translation = Vec3::FromGLM(glmTranslation);
        rotation    = Quat::FromGLM(glmRotation);
        scale       = Vec3::FromGLM(glmScale);

        return true;
    }

    int FindMax(int firstNum, int secondNum) {
        return std::max(firstNum, secondNum);
    }

    int FindClosestPowerOfTwo(int num) {
        int x = 1, y = 0;
        while (x < num) {
            y = x;
            x <<= 1;
        }
        return abs(y - num) > abs(x - num) ? x : y;
    }

    Quat LookRotation(const Vec3 &fwd, const Vec3 &up) noexcept {
        // Ensure forward is normalized
        const Vec3 f = fwd.Normalized();
        // Ensure up is not parallel to forward
        const Vec3 r = up.Cross(f).Normalized();
        // Recalculate orthonormal up
        const Vec3 u = f.Cross(r);

        // Create rotation matrix from orthonormal basis
        const float m00 = r.x, m01 = r.y, m02 = r.z;
        const float m10 = u.x, m11 = u.y, m12 = u.z;
        const float m20 = f.x, m21 = f.y, m22 = f.z;

        // Convert matrix to quaternion (trace method)
        const float trace = m00 + m11 + m22;
        Quat q{};

        if (trace > 0.0f) {
            const float s = sqrt(trace + 1.0f) * 2.0f;
            const float invS = 1.0f / s;
            q.w = 0.25f * s;
            q.x = (m12 - m21) * invS;
            q.y = (m20 - m02) * invS;
            q.z = (m01 - m10) * invS;
        } else if (m00 > m11 && m00 > m22) {
            const float s = sqrt(1.0f + m00 - m11 - m22) * 2.0f;
            const float invS = 1.0f / s;
            q.w = (m12 - m21) * invS;
            q.x = 0.25f * s;
            q.y = (m01 + m10) * invS;
            q.z = (m02 + m20) * invS;
        } else if (m11 > m22) {
            const float s = sqrt(1.0f + m11 - m00 - m22) * 2.0f;
            const float invS = 1.0f / s;
            q.w = (m20 - m02) * invS;
            q.x = (m01 + m10) * invS;
            q.y = 0.25f * s;
            q.z = (m12 + m21) * invS;
        } else {
            const float s = sqrt(1.0f + m22 - m00 - m11) * 2.0f;
            const float invS = 1.0f / s;
            q.w = (m01 - m10) * invS;
            q.x = (m02 + m20) * invS;
            q.y = (m12 + m21) * invS;
            q.z = 0.25f * s;
        }

        return q.Normalized();
    }

    Vec3 DegreesToRadians(const Vec3 &deg) noexcept {
        return {
            DegreesToRadians(deg.x),
            DegreesToRadians(deg.y),
            DegreesToRadians(deg.z)
        };
    }

    Vec3 RadiansToDegrees(const Vec3 &rad) noexcept {
        return {
            RadiansToDegrees(rad.x),
            RadiansToDegrees(rad.y),
            RadiansToDegrees(rad.z)
        };
    }

    Vec3 ToEulerDegrees(const Quat &q) noexcept {
        const glm::vec3 radians = glm::eulerAngles(q.ToGLM());
        return RadiansToDegrees(Vec3::FromGLM(radians));
    }
}
