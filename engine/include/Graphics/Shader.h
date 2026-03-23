//
// Created by pointerlost on 10/4/25.
//
#pragma once
#include <unordered_map>
#include "RenderTypes.h"
#include "Common/Types.h"

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
        Shader(String vertexPath, String fragmentPath, String name) noexcept;
        Shader() = default;
        Shader(Shader&&) = default;
        Shader& operator=(Shader&&) = default;
        Shader(const Shader& shader) = default;
        Shader& operator=(const Shader&) = default;

        void SetInt(const String& name, int value) const noexcept ;
        void SetBool(const String& name, bool value) const noexcept ;
        void SetUint(const String& name, uint value) const noexcept ;
        void SetFloat(const String& name, f32 value) const noexcept ;
        void SetVec2(const String& name, const math::Vec2& value) const noexcept ;
        void SetVec2(const String& name, f32 x, f32 y) const noexcept ;
        void SetVec3(const String& name, const math::Vec3& value) const noexcept ;
        void SetVec3(const String& name, f32 x, f32 y, f32 z) const noexcept ;
        void SetVec4(const String& name, const math::Vec4& value) const noexcept ;
        void SetVec4(const String& name, f32 x, f32 y, f32 z, f32 w) const noexcept ;
        void SetMat2(const String& name, const math::Mat2& mat) const noexcept ;
        void SetMat3(const String& name, const math::Mat3& mat) const noexcept ;
        void SetMat4(const String& name, const math::Mat4& mat) const noexcept ;

        [[nodiscard]] const String& GetName() const noexcept { return m_Name; }
        [[nodiscard]] const graphics::ShaderHandle& GetProgram() const noexcept { return m_Program; }
        void Bind() const noexcept;

    private:
        graphics::ShaderHandle m_Program{};
        String m_VertexPath;
        String m_FragmentPath;
        String m_Name;

        mutable std::unordered_map<String, int> m_CacheUniforms;

    private:
        /* Get uniform location */
        [[nodiscard]] int GetULocation(const String& name) const noexcept;
        void CheckCompileErrors(graphics::ShaderHandle shader, const graphics::ShaderStage& stage) noexcept;
    };
}
