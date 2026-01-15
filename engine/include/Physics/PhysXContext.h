//
// Created by pointerlost on 1/13/26.
//
#pragma once
#include <PxPhysicsAPI.h>

#include "PhysXAllocator.h"
#include "PhysXErrorCallback.h"
#include "Core/Utils.h"

namespace Real::ecs {

    // This class manages: PhysX lifetime and low-level state
    class PhysXContext {
    public:
        void Init();
        void Shutdown();

        [[nodiscard]] physx::PxPhysics& Physics() const { return *m_Physics; }
        [[nodiscard]] physx::PxScene&   Scene()   const { return *m_Scene; }
        [[nodiscard]] physx::PxMaterial& DefaultMaterial() const { return *m_DefaultMaterial; }

    private:
        physx::PxFoundation*    m_Foundation = nullptr;
        physx::PxPhysics*       m_Physics = nullptr;
        physx::PxScene*         m_Scene = nullptr;
        physx::PxCpuDispatcher* m_Dispatcher = nullptr;
        physx::PxMaterial*      m_DefaultMaterial = nullptr;

        Scope<physics::PhysXAllocator>      m_Allocator;
        Scope<physics::PhysXErrorCallback>  m_ErrorCallback;
    };
}
