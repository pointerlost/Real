//
// Created by pointerlost on 10/4/25.
//
#pragma once
#include <unordered_map>
#include "Common/RealTypes.h"
#include "glad/glad.h"

namespace Real::math {
    struct Mat4;
    struct Mat2;
    struct Vec4;
    struct Vec3;
    struct Vec2;
    struct Mat3;
}

namespace Real {

    class Shader {
    public:
        Shader(String vertexPath, String fragmentPath, String name);
        Shader() = default;
        Shader(Shader&&) = default;
        Shader& operator=(Shader&&) = default;
        Shader(const Shader& shader) = default;
        Shader& operator=(const Shader&) = default;

        void SetInt(const String& name, int value) const;
        void SetBool(const String& name, bool value) const;
        void SetUint(const String& name, uint value) const;
        void Setf32(const String& name, f32 value) const;
        void SetVec2(const String& name, const math::Vec2& value) const;
        void SetVec2(const String& name, f32 x, f32 y) const;
        void SetVec3(const String& name, const math::Vec3& value) const;
        void SetVec3(const String& name, f32 x, f32 y, f32 z) const;
        void SetVec4(const String& name, const math::Vec4& value) const;
        void SetVec4(const String& name, f32 x, f32 y, f32 z, f32 w) const;
        void SetMat2(const String& name, const math::Mat2& mat) const;
        void SetMat3(const String& name, const math::Mat3& mat) const;
        void SetMat4(const String& name, const math::Mat4& mat) const;

        [[nodiscard]] const String& GetName() const { return m_Name; }
        [[nodiscard]] const ShaderHandle& GetProgram() const { return m_Program; }
        void Bind() const;

    private:
        ShaderHandle m_Program{};
        String m_VertexPath;
        String m_FragmentPath;
        String m_Name;

        mutable std::unordered_map<String, int> m_CacheUniforms;

    private:
        /* Get uniform location */
        [[nodiscard]] int GetULocation(const String& name) const;
        void CheckCompileErrors(ShaderHandle shader, const String& type);
    };
}
