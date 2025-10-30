//
// Created by pointerlost on 10/29/25.
//
#include "API/OpenGL/Types/FrameBuffer.h"

#include "Core/Logger.h"
#include "Core/Utils.h"

namespace Real::opengl {

    FrameBuffer::FrameBuffer(const std::string &name, int width, int height) {
    }

    FrameBuffer::FrameBuffer(const std::string &name, const glm::ivec2 &screenSize) {
    }

    FrameBuffer::~FrameBuffer() {
        CleanUp();
    }

    void FrameBuffer::Create(const std::string &name, int width, int height) {
    }

    void FrameBuffer::Create(const std::string &name, const glm::ivec2 &resolution) {
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

    void FrameBuffer::CleanUp() {
        glDeleteFramebuffers(1, &m_FBO);
    }
}
