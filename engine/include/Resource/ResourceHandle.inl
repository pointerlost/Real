//
// Created by pointerlost on 3/29/26.
//
#ifndef REAL_RESOURCE_HANDLE_INL
#define REAL_RESOURCE_HANDLE_INL
#include "Core/IResourceManager.h"

namespace Real::core {

    template<typename T>
    ResourceHandle<T>::~ResourceHandle() {
        // TODO: enable when ref counting implemented
        // if (m_ResourceManager && !m_Handle.IsNull())
        //     m_ResourceManager->Release(m_Handle);
    }

    template<typename T>
    T* ResourceHandle<T>::Get() const {
        if (!IsValid() || !m_ResourceManager)
            return nullptr;
        return static_cast<T*>(m_ResourceManager->Get(m_Handle));
    }
}

#endif