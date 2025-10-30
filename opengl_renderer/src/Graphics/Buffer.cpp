//
// Created by pointerlost on 10/12/25.
//
#include "Graphics/Buffer.h"

namespace Real::opengl {

    Buffer::~Buffer() {
        CleanResources();
    }

    void Buffer::Bind(GLenum target, BufferType type, GLuint bindingPoint) const {
        if (type == BufferType::SSBO) {
            glBindBufferBase(target, bindingPoint, m_Buffer);
        } else if (type == BufferType::UBO) {
            glBindBufferBase(target, bindingPoint, m_Buffer);
        }
    }

    void Buffer::CleanResources() {
        if (m_Ptr) {
            glUnmapNamedBuffer(m_Buffer);
            m_Ptr = nullptr;
        }
        glDeleteBuffers(1, &m_Buffer);
    }
}
