//
// Created by pointerlost on 10/4/25.
//
#pragma once
#include <iostream>
#include <unordered_map>
#include <glad/glad.h>
#include <glm/ext.hpp>

namespace Real {

    class Shader {
    public:
        Shader(std::string vertexPath, std::string fragmentPath, std::string name);
        Shader() = default;
        Shader(Shader&&) = default;
        Shader& operator=(Shader&&) = default;
        Shader(const Shader& shader) = default;
        Shader& operator=(const Shader&) = default;

        void SetInt(const std::string& name, int value) const;
        void SetBool(const std::string& name, bool value) const;
        void SetUint(const std::string& name, uint value) const;
        void SetFloat(const std::string& name, float value) const;
        void SetVec2(const std::string& name, const glm::vec2& value) const;
        void SetVec2(const std::string& name, float x, float y) const;
        void SetVec3(const std::string& name, const glm::vec3& value) const;
        void SetVec3(const std::string& name, float x, float y, float z) const;
        void SetVec4(const std::string& name, const glm::vec4& value) const;
        void SetVec4(const std::string& name, float x, float y, float z, float w) const;
        void SetMat2(const std::string& name, const glm::mat2& mat) const;
        void SetMat3(const std::string& name, const glm::mat3& mat) const;
        void SetMat4(const std::string& name, const glm::mat4& mat) const;

        [[nodiscard]] const std::string& GetName() const { return m_Name; }
        [[nodiscard]] const GLuint& GetProgram() const { return m_Program; }
        void Bind() const { glUseProgram(m_Program); }

    private:
        GLuint m_Program;
        std::string m_VertexPath;
        std::string m_FragmentPath;
        std::string m_Name;

        mutable std::unordered_map<std::string, int> m_CacheUniforms;

    private:
        /* Get uniform location */
        [[nodiscard]] int GetULocation(const std::string& name) const;
        void CheckCompileErrors(GLuint shader, std::string type);
    };
}
