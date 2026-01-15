//
// Created by pointerlost on 1/13/26.
//
#include <Physics/PhysXContext.h>
#include <physx/include/PxPhysicsAPI.h>

namespace Real::ecs {

    void PhysXContext::Init() {
        using namespace physx;

        // Create PhysX allocator and error callback (must init for Backend)
        m_Allocator     = CreateScope<physics::PhysXAllocator>();
        m_ErrorCallback = CreateScope<physics::PhysXErrorCallback>();

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

        // Scene with collision detection enabled
        PxSceneDesc sceneDesc(m_Physics->getTolerancesScale());
        sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
        m_Scene = m_Physics->createScene(sceneDesc);
    }

    void PhysXContext::Shutdown() {
        m_Scene = nullptr;
    }
}
