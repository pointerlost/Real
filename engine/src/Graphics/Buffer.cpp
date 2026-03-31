//
// Created by pointerlost on 10/12/25.
//
#include "Graphics/Buffer.h"
#include <algorithm>
#include <cstring>
#include <glad/include/glad/glad.h>

namespace Real::opengl {

    Buffer::~Buffer() noexcept {
        Destroy();
    }

    // Proper move constructor (no delegation)
    Buffer::Buffer(Buffer&& other) noexcept
        : m_Buffer(other.m_Buffer),
          m_MappedPtr(other.m_MappedPtr),
          m_Size(other.m_Size),
          m_Type(other.m_Type)
    {
        other.m_Buffer    = graphics::BufferHandle{};
        other.m_MappedPtr = nullptr;
        other.m_Size      = 0;
    }

    // Move assignment
    Buffer& Buffer::operator=(Buffer&& other) noexcept {
        if (this != &other) {
            Destroy();

            m_Buffer    = other.m_Buffer;
            m_MappedPtr = other.m_MappedPtr;
            m_Size      = other.m_Size;
            m_Type      = other.m_Type;

            other.m_Buffer    = graphics::BufferHandle{};
            other.m_MappedPtr = nullptr;
            other.m_Size      = 0;
        }
        return *this;
    }

    void Buffer::Create(const void* data, size_t size, graphics::BufferType type) {
        Destroy();

        m_Type = type;
        Allocate(size);

        if (data && size > 0) {
            Upload(data, size);
        }
    }

    void Buffer::Allocate(size_t size) {
        m_Size = size;

        glCreateBuffers(1, &m_Buffer.value);

        if (m_Type == graphics::BufferType::SSBO) {

            constexpr GLbitfield storageFlags =
                GL_DYNAMIC_STORAGE_BIT |
                GL_MAP_WRITE_BIT |
                GL_MAP_PERSISTENT_BIT |
                GL_MAP_FLUSH_EXPLICIT_BIT;

            glNamedBufferStorage(m_Buffer.value, size, nullptr, storageFlags);

            constexpr GLbitfield mapFlags =
                GL_MAP_WRITE_BIT |
                GL_MAP_PERSISTENT_BIT |
                GL_MAP_FLUSH_EXPLICIT_BIT;

            m_MappedPtr = glMapNamedBufferRange(m_Buffer.value, 0, size, mapFlags);
        }
        else if (m_Type == graphics::BufferType::UBO) {

            glNamedBufferStorage(m_Buffer.value, size, nullptr, GL_DYNAMIC_STORAGE_BIT);

            m_MappedPtr = nullptr;
        }
    }

    void Buffer::Upload(const void* data, size_t size) {
        if (!m_Buffer.IsValid() || !data || size == 0)
            return;

        if (size > m_Size) {
            size_t newSize = (m_Size == 0) ? size : std::max(size, m_Size * 2);
            Resize(newSize);
        }

        if (m_Type == graphics::BufferType::SSBO && m_MappedPtr) {
            std::memcpy(m_MappedPtr, data, size);
            glFlushMappedNamedBufferRange(m_Buffer.value, 0, size);
        }
        else {
            glNamedBufferSubData(m_Buffer.value, 0, size, data);
        }
    }

    void Buffer::Resize(size_t newSize) {
        const graphics::BufferHandle oldBuffer = m_Buffer;
        const void* oldMapped = m_MappedPtr;
        const size_t oldSize = m_Size;

        // Allocate new buffer (safety)
        m_Buffer    = graphics::BufferHandle{};
        m_MappedPtr = nullptr;
        m_Size      = 0;

        Allocate(newSize);

        // If old existed, copy data
        if (oldBuffer.IsValid()) {
            glCopyNamedBufferSubData(
                oldBuffer.value,
                m_Buffer.value,
                0,
                0,
                static_cast<GLsizeiptr>(std::min(oldSize, newSize))
            );

            if (oldMapped)
                glUnmapNamedBuffer(oldBuffer.value);

            glDeleteBuffers(1, &oldBuffer.value);
        }
    }

    void Buffer::Bind(uint32_t target, graphics::BindingPoint bindingPoint) const noexcept {
        glBindBufferBase(target, bindingPoint.value, m_Buffer.value);
    }

    void Buffer::Destroy() noexcept {
        if (m_Buffer.IsValid()) {
            if (m_MappedPtr)
                glUnmapNamedBuffer(m_Buffer.value);
            glDeleteBuffers(1, &m_Buffer.value);
        }

        m_Buffer    = graphics::BufferHandle{};
        m_MappedPtr = nullptr;
        m_Size      = 0;
        m_Type      = graphics::BufferType{};
    }

}
