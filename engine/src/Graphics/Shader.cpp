//
// Created by pointerlost on 10/4/25.
//
#include "Graphics/Shader.h"
#include <utility>
#include "Core/Logger.h"
#include "glad/include/glad/glad.h"
#include "Graphics/RenderTypes.h"
#include "Math/Mat2.h"
#include "Math/Mat3.h"
#include "Math/Mat4.h"
#include "Math/Vec2.h"
#include "Math/Vec3.h"
#include "Math/Vec4.h"

namespace Real::graphics {

    Shader::Shader(String vertexPath, String fragmentPath, const ShaderType& type) noexcept
        : m_Type(type),
          m_VertexPath(std::move(vertexPath)),
          m_FragmentPath(std::move(fragmentPath))
    {
        const char* vSource = m_VertexPath.c_str();
        const char* fSource = m_FragmentPath.c_str();

        const ShaderHandle vertex{glCreateShader(GL_VERTEX_SHADER)};
        glShaderSource(vertex.value, 1, &vSource, nullptr);
        glCompileShader(vertex.value);
        CheckCompileErrors(vertex, ShaderStage::Vertex);

        const ShaderHandle fragment{glCreateShader(GL_FRAGMENT_SHADER)};
        glShaderSource(fragment.value, 1, &fSource, nullptr);
        glCompileShader(fragment.value);
        CheckCompileErrors(fragment, ShaderStage::Fragment);

        m_Program.value = glCreateProgram();
        glAttachShader(m_Program.value, vertex.value);
        glAttachShader(m_Program.value, fragment.value);
        glLinkProgram(m_Program.value);
        CheckCompileErrors(m_Program, ShaderStage::Program);
    }

    void Shader::SetInt(const String &name, int value) const noexcept {
        glUniform1i(GetULocation(name), static_cast<int>(value));
    }

    void Shader::SetBool(const String &name, bool value) const noexcept {
        glUniform1i(GetULocation(name), static_cast<int>(value));
    }

    void Shader::SetUint(const String &name, uint value) const noexcept {
        glUniform1f(GetULocation(name), static_cast<float>(value));
    }

    void Shader::SetFloat(const String &name, f32 value) const noexcept {
        glUniform1f(GetULocation(name), value);
    }

    void Shader::SetVec2(const String &name, const math::Vec2& value) const noexcept {
        glUniform2fv(GetULocation(name), 1, value.ValuePtr());
    }

    void Shader::SetVec2(const String &name, f32 x, f32 y) const noexcept {
        glUniform2f(GetULocation(name), x, y);
    }

    void Shader::SetVec3(const String &name, const math::Vec3 &value) const noexcept {
        glUniform3fv(GetULocation(name), 1, value.ValuePtr());
    }

    void Shader::SetVec3(const String &name, f32 x, f32 y, f32 z) const noexcept {
        glUniform3f(GetULocation(name), x, y, z);
    }

    void Shader::SetVec4(const String &name, const math::Vec4& value) const noexcept {
        glUniform4fv(GetULocation(name), 1, value.ValuePtr());
    }

    void Shader::SetVec4(const String &name, f32 x, f32 y, f32 z, f32 w) const noexcept {
        glUniform4f(GetULocation(name), x, y, z, w);
    }

    void Shader::SetMat2(const String &name, const math::Mat2& mat) const noexcept {
        glUniformMatrix2fv(GetULocation(name), 1, GL_FALSE, mat.ValuePtr());
    }

    void Shader::SetMat3(const String &name, const math::Mat3& mat) const noexcept {
        glUniformMatrix3fv(GetULocation(name), 1, GL_FALSE, mat.ValuePtr());
    }

    void Shader::SetMat4(const String &name, const math::Mat4& mat) const noexcept {
        glUniformMatrix4fv(GetULocation(name), 1, GL_FALSE, mat.ValuePtr());
    }

    void Shader::Bind() const noexcept {
        glUseProgram(m_Program.value);
    }

    int Shader::GetULocation(const String& name) const noexcept {
        if (m_CacheUniforms.contains(name)) {
            return m_CacheUniforms[name];
        }
        const int loc = glGetUniformLocation(m_Program.value, name.c_str());
        m_CacheUniforms[name] = loc;
        return loc;
    }

    void Shader::CheckCompileErrors(ShaderHandle shader, const ShaderStage& stage) noexcept {
        GLint success;
        GLchar infoLog[1024];
        switch (stage) {
            case ShaderStage::Program: {
                glGetShaderiv(shader.value, GL_COMPILE_STATUS, &success);
                if (!success) {
                    glGetShaderInfoLog(shader.value, 1024, nullptr, infoLog);
                    Warn("ERROR::SHADER_COMPILATION_FAILED_ERROR of type: (id) = " + std::to_string((int)stage));
                    Warn(infoLog);
                }
            }

            default: {
                glGetProgramiv(shader.value, GL_LINK_STATUS, &success);
                if (!success) {
                    glGetProgramInfoLog(shader.value, 1024, nullptr, infoLog);
                    Warn("ERROR::PROGRAM_LINKING_ERROR of type: (id) = " + std::to_string((int)stage));
                    Warn(infoLog);
                }
            }
        }

    }
}
