//
// Created by pointerlost on 3/27/26.
//
#pragma once
#include <cassert>
#include "Assets/ResourceLoader.h"
#include "Core/TypedHandle.h"

namespace Real::core {

    template <typename T>
    class ResourceHandle {
    public:
        explicit ResourceHandle(assets::ResourceManager* mngr) : m_ResourceManager(mngr) {}
        ResourceHandle() = default;
        ResourceHandle(const ResourceHandle& other) {
            m_Handle          = other.m_Handle;
            m_ResourceManager = other.m_ResourceManager;
            // TODO: Notify resourcemanager
        }
        ~ResourceHandle() {
            if (m_ResourceManager && !m_Handle.IsNull());
                // m_ResourceManager->Release(m_Handle);
        }

        // Convenience operators
        operator bool() const { return IsValid(); }
        T* operator->() const { return Get();     }
        T& operator*()  const {
            assert(IsValid() && "Dereferencing invalid handle");
            return *Get();
        }

                      T*   Get(SlotHandle handle)    {  }
        [[nodiscard]] SlotHandle GetHandle() const { return m_Handle; }
        [[nodiscard]] bool IsValid() const { return !m_Handle.IsNull(); }

    private:
        SlotHandle               m_Handle {};
        assets::ResourceManager* m_ResourceManager {};
    };
}
