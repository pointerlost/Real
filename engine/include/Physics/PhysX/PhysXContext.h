//
// Created by pointerlost on 1/13/26.
//
#pragma once
#include "PhysXAllocator.h"
#include "PhysXErrorCallback.h"
#include "Common/Utils.h"

namespace physx {
    class PxCpuDispatcher;
    class PxFoundation;
    class PxMaterial;
    class PxScene;
    class PxPhysics;
}

namespace Real::physics {

    // This class manages: PhysX lifetime and low-level state
    class PhysXContext {
    public:
        void Init();
        void Shutdown();

        [[nodiscard]] physx::PxPhysics&  GetPhysics()         const { return *m_Physics;         }
        [[nodiscard]] physx::PxScene&    GetScene()           const { return *m_Scene;           }
        [[nodiscard]] physx::PxMaterial& GetDefaultMaterial() const { return *m_DefaultMaterial; }

    private:
        physx::PxFoundation*    m_Foundation      = nullptr;
        physx::PxPhysics*       m_Physics         = nullptr;
        physx::PxScene*         m_Scene           = nullptr;
        physx::PxCpuDispatcher* m_Dispatcher      = nullptr;
        physx::PxMaterial*      m_DefaultMaterial = nullptr;

        Scope<PhysXAllocator>     m_Allocator;
        Scope<PhysXErrorCallback> m_ErrorCallback;
    };
}
