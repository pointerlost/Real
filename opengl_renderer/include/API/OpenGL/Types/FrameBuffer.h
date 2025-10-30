//
// Created by pointerlost on 10/29/25.
//
#pragma once
#include <string>
#include <glad/include/glad/glad.h>
#include <glm/vec2.hpp>

namespace Real::opengl {

    // TODO: Create Color Buffer

    // TODO: Create Depth Buffer

    // TODO: Create Stencil Buffer

    class FrameBuffer {
    public:
        FrameBuffer() = default;
        FrameBuffer(const std::string& name, int width, int height);
        FrameBuffer(const std::string& name, const glm::ivec2& resolution);
        ~FrameBuffer();
        FrameBuffer(const FrameBuffer&) = default;

        void Create(const std::string& name, int width, int height);
        void Create(const std::string& name, const glm::ivec2& resolution);
        void Bind() const;
        void Unbind();
        void CheckIfStatusCompleted();

        [[nodiscard]] GLuint GetHandle() const { return m_FBO; }
        [[nodiscard]] glm::ivec2 GetViewportSize() const { return glm::ivec2{m_Width, m_Height}; }
        [[nodiscard]] const std::string& GetName() const { return m_Name; }

    private:
        void CleanUp();

    private:
        GLuint m_FBO = 0;
        int m_Width = 800;
        int m_Height = 600;
        std::string m_Name = "null";
    };
}
