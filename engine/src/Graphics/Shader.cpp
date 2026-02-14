//
// Created by pointerlost on 10/4/25.
//
#include "Graphics/Shader.h"
#include <utility>
#include "Core/Logger.h"
#include "Math/Mat2.h"
#include "Math/Mat3.h"
#include "Math/Mat4.h"
#include "Math/Vec2.h"
#include "Math/Vec3.h"
#include "Math/Vec4.h"

namespace Real {

    Shader::Shader(String vertexPath, String fragmentPath, String name)
        : m_VertexPath(std::move(vertexPath)), m_FragmentPath(std::move(fragmentPath)), m_Name(std::move(name))
    {
        const char* vSource = m_VertexPath.c_str();
        const char* fSource = m_FragmentPath.c_str();

        const auto vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vSource, nullptr);
        glCompileShader(vertex);
        CheckCompileErrors(vertex, "VERTEX");

        const auto fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fSource, nullptr);
        glCompileShader(fragment);
        CheckCompileErrors(fragment, "FRAGMENT");

        m_Program = glCreateProgram();
        glAttachShader(m_Program, vertex);
        glAttachShader(m_Program, fragment);
        glLinkProgram(m_Program);
        CheckCompileErrors(m_Program, "PROGRAM");
    }

    void Shader::SetInt(const String &name, int value) const {
        glUniform1i(GetULocation(name), (int)value);
    }

    void Shader::SetBool(const String &name, bool value) const {
        glUniform1i(GetULocation(name), (int)value);
    }

    void Shader::SetUint(const String &name, uint value) const {
        glUniform1f(GetULocation(name), (uint)value);
    }

    void Shader::Setf32(const String &name, f32 value) const {
        glUniform1f(GetULocation(name), value);
    }

    void Shader::SetVec2(const String &name, const math::Vec2& value) const {
        glUniform2fv(GetULocation(name), 1, value.ValuePtr());
    }

    void Shader::SetVec2(const String &name, f32 x, f32 y) const {
        glUniform2f(GetULocation(name), x, y);
    }

    void Shader::SetVec3(const String &name, const math::Vec3 &value) const {
        glUniform3fv(GetULocation(name), 1, value.ValuePtr());
    }

    void Shader::SetVec3(const String &name, f32 x, f32 y, f32 z) const {
        glUniform3f(GetULocation(name), x, y, z);
    }

    void Shader::SetVec4(const String &name, const math::Vec4& value) const {
        glUniform4fv(GetULocation(name), 1, value.ValuePtr());
    }

    void Shader::SetVec4(const String &name, f32 x, f32 y, f32 z, f32 w) const {
        glUniform4f(GetULocation(name), x, y, z, w);
    }

    void Shader::SetMat2(const String &name, const math::Mat2& mat) const {
        glUniformMatrix2fv(GetULocation(name), 1, GL_FALSE, mat.ValuePtr());
    }

    void Shader::SetMat3(const String &name, const math::Mat3& mat) const {
        glUniformMatrix3fv(GetULocation(name), 1, GL_FALSE, mat.ValuePtr());
    }

    void Shader::SetMat4(const String &name, const math::Mat4& mat) const {
        glUniformMatrix4fv(GetULocation(name), 1, GL_FALSE, mat.ValuePtr());
    }

    void Shader::Bind() const {
        glUseProgram(m_Program);
    }

    int Shader::GetULocation(const String &name) const {
        if (m_CacheUniforms.contains(name)) {
            return m_CacheUniforms[name];
        }
        const int loc = glGetUniformLocation(m_Program, name.c_str());
        m_CacheUniforms[name] = loc;
        return loc;
    }

    void Shader::CheckCompileErrors(GLuint shader, const String& type) {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
                Warn("ERROR::SHADER_COMPILATION_FAILED_ERROR of type: " + type);
                Warn(infoLog);
            }
        }
        else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
                Warn("ERROR::PROGRAM_LINKING_ERROR of type: " + type);
                Warn(infoLog);
            }
        }
    }
}
