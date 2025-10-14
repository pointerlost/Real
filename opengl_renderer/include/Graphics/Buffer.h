//
// Created by pointerlost on 10/12/25.
//
#pragma once
#include <cstring>
#include <iostream>
#include <vector>

#include "Core/Logger.h"
#include "glad/glad.h"

namespace Real::opengl {

    enum class BufferType {
        SSBO,
        UBO,
    };

    struct Buffer {
        Buffer() = default;
        Buffer(const Buffer&) = default;
        ~Buffer();

        [[nodiscard]] GLuint GetHandle() const {
            if (m_Buffer == 0) { Warn("Buffer doesn't exists! from: " + std::string(__FILE__)); return 0; }
            return m_Buffer;
        }

        template <typename T>
        void Create(const std::vector<T>& data, GLsizeiptr size, BufferType type) {
            m_Size = size;
            CleanResources();
            Create(data, type);
        }

        template <typename T>
        void UploadToGPU(const std::vector<T>& data, GLsizeiptr size, BufferType type) {
            if (data.empty()) return;
            if (type == BufferType::SSBO) {
                if (m_Size <= size) {
                    m_Size *= 2;
                    if (m_Ptr) {
                        Create(data, type);
                    }
                }
                else {
                    if (m_Ptr) {
                        memcpy(m_Ptr, data.data(), m_Size);
                        glFlushMappedNamedBufferRange(m_Buffer, 0, m_Size);
                    }
                }
            }
            // else if (type == BufferType::UBO) {
            //     // TODO: Need update for resizing!! (for now, it will have one element) !!!
            //     if (m_Buffer != 0) {
            //         m_Size = size;
            //         glNamedBufferStorage(m_Buffer, m_Size, reinterpret_cast<const void *>(data.data()),
            //             GL_DYNAMIC_STORAGE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT
            //         );
            //     } else {
            //         Create(data, size, type);
            //     }
            // }
        }

        void Bind(BufferType type, GLuint bindingPoint) const;

    private:
        GLuint m_Buffer = 0;
        void* m_Ptr = nullptr;
        GLsizeiptr m_Size = 0;
        GLbitfield m_Flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT;

    private:
        template <typename T>
        void Create(const std::vector<T>& data, BufferType type) {
            if (type == BufferType::SSBO) {
                glCreateBuffers(1, &m_Buffer);
                if (m_Buffer == 0) {
                    Warn("Buffer creation failed from: " + std::string(__FILE__));
                    return;
                }
                // Direct State Access
                glNamedBufferStorage(m_Buffer, m_Size, (const void*)data.data(),
                    GL_DYNAMIC_STORAGE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT
                );
                m_Ptr = glMapNamedBufferRange(m_Buffer, 0, m_Size, m_Flags);
                if (!m_Ptr) {
                    Warn("Persistent mapping pointer nullptr from: " + std::string(__FILE__));
                    return;
                }
                // memcpy(m_Ptr, data.data(), m_Size);
                // glFlushMappedNamedBufferRange(m_Buffer, 0, m_Size);
            }
            // else if (type == BufferType::UBO) {
            //     glCreateBuffers(1, &m_Buffer);
            //     glNamedBufferStorage(m_Buffer, m_Size, reinterpret_cast<const void *>(data.data()), GL_DYNAMIC_STORAGE_BIT);
            //     glFlushMappedNamedBufferRange(m_Buffer, 0, m_Size);
            // }
        }

        void CleanResources();
    };

}
