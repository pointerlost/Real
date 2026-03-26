//
// Created by pointerlost on 10/29/25.
//
#include "Platform/OpenGL/OpenGLFrameBuffer.h"
#include "Core/Logger.h"
#include <utility>

#include "Common/Utils.h"
#include "glad/glad.h"
#include "Math/iVec2.h"

namespace Real::opengl {

    OpenGLFrameBuffer::OpenGLFrameBuffer(String name, int width, int height)
        : m_Width(width), m_Height(height), m_Name(std::move(name))
    {
    }

    OpenGLFrameBuffer::OpenGLFrameBuffer(String name, const math::iVec2& resolution)
        : m_Name(std::move(name)), m_Width(resolution.x), m_Height(resolution.y)
    {
    }

    OpenGLFrameBuffer::~OpenGLFrameBuffer() {
        CleanUp();
    }

    void OpenGLFrameBuffer::Create() {
    }

    void OpenGLFrameBuffer::Bind() const {
        if (m_FBO.IsValid())
            glBindFramebuffer(GL_FRAMEBUFFER, m_FBO.value);
        else
            Warn("There is no OpenGL framebuffer to bind!");
    }

    void OpenGLFrameBuffer::Unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OpenGLFrameBuffer::CheckIfStatusCompleted() {
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            Warn(ConcatStr("GL FrameBuffer can't created! from: ", __FILE__));
        }
        Info("GL FrameBuffer created successfully!");
    }

    void OpenGLFrameBuffer::SetTarget(graphics::BufferHandle target) {
        m_Target = target;
    }

    void OpenGLFrameBuffer::BindAttachment(const graphics::BufferHandle& attachment) const {
        glFramebufferTexture2D(m_Target.value, attachment.value, GL_TEXTURE_2D, m_Texture.value, NULL);
    }

    math::iVec2 OpenGLFrameBuffer::GetViewportSize() const {
        { return math::iVec2{m_Width, m_Height}; }
    }

    void OpenGLFrameBuffer::CleanUp() const {
        if (m_FBO.IsValid())
            glDeleteFramebuffers(1, &m_FBO.value);
    }
}
