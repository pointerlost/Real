//
// Created by pointerlost on 10/29/25.
//
#include "API/OpenGL/Types/FrameBuffer.h"

#include <utility>

#include "Core/Logger.h"
#include "Core/Utils.h"

namespace Real::opengl {

    FrameBuffer::FrameBuffer(std::string name, int width, int height)
        : m_Width(width), m_Height(height), m_Name(std::move(name))
    {
    }

    FrameBuffer::FrameBuffer(std::string name, const glm::ivec2 &screenSize)
        : m_Width(screenSize.x), m_Height(screenSize.y), m_Name(std::move(name))
    {
    }

    FrameBuffer::~FrameBuffer() {
        CleanUp();
    }

    void FrameBuffer::Create() {
    }

    void FrameBuffer::Bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    }

    void FrameBuffer::Unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FrameBuffer::CheckIfStatusCompleted() {
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            // Execute victory dance
            Warn(ConcatStr("GL FrameBuffer can't created! from: ", __FILE__));
        }
        Info("GL FrameBuffer created successfully!");
    }

    void FrameBuffer::SetTarget(GLenum target) {
        m_Target = target;
    }

    void FrameBuffer::BindAttachment(GLenum attachment) {
        glFramebufferTexture2D(m_Target, attachment, GL_TEXTURE_2D, m_Texture, NULL);
    }

    void FrameBuffer::CleanUp() {
        glDeleteFramebuffers(1, &m_FBO);
    }
}
