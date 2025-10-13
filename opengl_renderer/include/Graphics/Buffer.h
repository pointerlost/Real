//
// Created by pointerlost on 10/12/25.
//
#pragma once
#include <cstring>
#include <vector>
#include "glad/glad.h"

namespace Real::opengl {

    enum class BufferType {
        SSBO,
        UBO,
    };

    struct Buffer {
        explicit Buffer(GLbitfield flags = GL_DYNAMIC_STORAGE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT);
        Buffer() = delete;
        Buffer(const Buffer&) = delete;
        ~Buffer();

        template <typename T>
        void Create(const std::vector<T>& data, GLsizeiptr size) {
            m_Size = size;
            CleanResources();
            Create(data);
        }

        template <typename T>
        void UploadToGPU(const std::vector<T>& data, GLsizeiptr size) {
            if (m_Size <= size) {
                m_Size *= 2;
                if (m_Ptr) {
                    Create(data);
                }
            }
            else {
                if (m_Ptr) {
                    memcpy(m_Ptr, data.data(), m_Size);
                    glFlushMappedNamedBufferRange(m_Buffer, 0, m_Size);
                }
            }
        }

        void Bind(BufferType type, GLuint bindingPoint);

    private:
        GLuint m_Buffer = 0;
        void* m_Ptr = nullptr;
        GLsizeiptr m_Size = 0;
        GLbitfield m_Flags = GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT;

    private:
        template <typename T>
        void Create(const std::vector<T>& data) {
            glCreateBuffers(1, &m_Buffer);
            // Direct State Access
            glNamedBufferStorage(m_Buffer, m_Size, reinterpret_cast<const void *>(data.data()), m_Flags);
            m_Ptr = glMapNamedBufferRange(m_Buffer, 0, m_Size, m_Flags);
            memcpy(m_Ptr, data.data(), m_Size);
            glFlushMappedNamedBufferRange(m_Buffer, 0, m_Size);
        }

        void CleanResources();
    };

}
