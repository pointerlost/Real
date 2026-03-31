//
// Created by pointerlost on 3/27/26.
//
#pragma once
#include <cassert>
#include "Assets/ResourceLoader.h"
#include "Core/TypedHandle.h"

namespace Real::assets {
    struct IResourceManager;
}

namespace Real::core {

    template<typename T>
    class ResourceHandle {
    public:
        ResourceHandle() = default;
        explicit ResourceHandle(assets::IResourceManager* mgr, SlotHandle handle)
            : m_Handle(handle), m_ResourceManager(mgr) {}

        // Copy - shallow, no ref count yet (TODO: when ref counting enabled)
        // TODO: add ref counter to release unused textures!
        ResourceHandle(const ResourceHandle& other)            = default;
        ResourceHandle& operator=(const ResourceHandle& other) = default;

        // Move - transfer ownership, null source
        ResourceHandle(ResourceHandle&& other) noexcept
            : m_Handle(other.m_Handle), m_ResourceManager(other.m_ResourceManager)
        { other.m_Handle = {}; }

        ResourceHandle& operator=(ResourceHandle&& other) noexcept {
            if (this != &other) {
                m_Handle          = other.m_Handle;
                m_ResourceManager = other.m_ResourceManager;
                other.m_Handle    = {};
            }
            return *this;
        }

        ~ResourceHandle();

        [[nodiscard]] T* Get() const;

        [[nodiscard]] SlotHandle GetHandle() const { return m_Handle;           }
        [[nodiscard]] bool       IsValid()   const { return !m_Handle.IsNull(); }

        explicit operator bool() const { return IsValid(); }
        T* operator->()          const { return Get();     }
        T& operator*()           const { assert(IsValid() && "Dereferencing invalid handle"); return *Get(); }

        bool operator==(const ResourceHandle& o) const { return m_Handle == o.m_Handle; }
        bool operator!=(const ResourceHandle& o) const { return m_Handle != o.m_Handle; }

    private:
        SlotHandle                m_Handle          {};
        assets::IResourceManager* m_ResourceManager {};
    };

}

#include "Resource/ResourceHandle.inl"

// Hash specialization
namespace std {
    template<typename T>
    struct hash<Real::core::ResourceHandle<T>> {
        size_t operator()(const Real::core::ResourceHandle<T>& h) const {
            return std::hash<Real::u32>{}(h.GetHandle().index)
                 ^ std::hash<Real::u32>{}(h.GetHandle().generation);
        }
    };
}
