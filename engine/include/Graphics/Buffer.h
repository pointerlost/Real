//
// Created by pointerlost on 10/12/25.
//
#pragma once
#include "RenderTypes.h"

namespace Real::opengl {

    class Buffer {
    public:
        Buffer() noexcept = default;
        ~Buffer() noexcept;

        // Non-copyable
        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;

        // Movable
        Buffer(Buffer&& other) noexcept;
        Buffer& operator=(Buffer&& other) noexcept;

        void Create(const void* data, size_t size, graphics::BufferType type);
        void Upload(const void* data, size_t size);

        void Bind(uint32_t target, graphics::BindingPoint bindingPoint) const noexcept;

        [[nodiscard]] graphics::BufferHandle GetHandle() const noexcept {
            return m_Buffer;
        }

        [[nodiscard]] size_t Size() const noexcept {
            return m_Size;
        }

    private:
        void Allocate(size_t size);
        void Resize(size_t newSize);
        void Destroy() noexcept;

    private:
        graphics::BufferHandle m_Buffer{};
        void* m_MappedPtr = nullptr;
        size_t m_Size = 0;
        graphics::BufferType m_Type{};
    };

}
