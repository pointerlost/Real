//
// Created by pointerlost on 1/13/26.
//
#include "Physics/PhysX/PhysXContext.h"
#include <PxPhysicsAPI.h>

namespace Real::physics {
    using namespace physx;

    void PhysXContext::Init()
    {
        // Create PhysX allocator and error callback (must init for Backend)
        m_Allocator     = CreateScope<PhysXAllocator>();
        m_ErrorCallback = CreateScope<PhysXErrorCallback>();

        m_Foundation = PxCreateFoundation(
            PX_PHYSICS_VERSION,
            *m_Allocator,
            *m_ErrorCallback
        );

        PX_ASSERT(m_Foundation);

        constexpr bool recordMemoryAllocations = true;
        m_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation,
            PxTolerancesScale(), recordMemoryAllocations, nullptr
        );

        m_DefaultMaterial = m_Physics->createMaterial(0.5f, 0.5f, 0.6f);

        // Scene with collision detection enabled
        PxSceneDesc sceneDesc(m_Physics->getTolerancesScale());

        sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);

        // REQUIRED
        sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(2);
        sceneDesc.filterShader  = PxDefaultSimulationFilterShader;

        PX_ASSERT(sceneDesc.isValid());

        m_Scene = m_Physics->createScene(sceneDesc);
    }

    void PhysXContext::Shutdown() {
        m_Scene = nullptr;
    }
}
