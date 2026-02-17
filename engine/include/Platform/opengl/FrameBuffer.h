//
// Created by pointerlost on 10/29/25.
//
#pragma once
#include <string>
#include <glad/include/glad/glad.h>
#include <glm/vec2.hpp>

#include "Common/RealTypes.h"

namespace Real::opengl {

    // TODO: Create Color Buffer

    // TODO: Create Depth Buffer

    // TODO: Create Stencil Buffer

    class FrameBuffer {
    public:
        FrameBuffer() = default;
        FrameBuffer(String name, int width, int height);
        FrameBuffer(String name, const glm::ivec2& resolution);
        FrameBuffer(const FrameBuffer&) = default;
        ~FrameBuffer();

        void Create();
        void Bind() const;
        void Unbind();
        void CheckIfStatusCompleted();
        void SetTarget(GLenum target);
        void BindAttachment(GLenum attachment);

        [[nodiscard]] GLuint GetHandle()           const { return m_FBO; }
        [[nodiscard]] glm::ivec2 GetViewportSize() const { return glm::ivec2{m_Width, m_Height}; }
        [[nodiscard]] const String& GetName() const { return m_Name; }
        [[nodiscard]] const GLenum& GetTarget()    const { return m_Target; }

    private:
        void CleanUp();

    private:
        GLuint m_FBO = 0;
        int m_Width = 800;
        int m_Height = 600;
        GLenum m_Target = GL_FRAMEBUFFER;
        String m_Name = "null";

        GLuint m_Texture = 0;
    };
}
