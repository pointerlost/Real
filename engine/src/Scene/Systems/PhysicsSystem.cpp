//
// Created by pointerlost on 1/14/26.
//
#include <Scene/Systems/PhysicsSystem.h>
#include "Core/Logger.h"

namespace Real::ecs {
    using namespace physx;

    void PhysicsSystem::Init() {
        m_PhysX = CreateScope<PhysXContext>();
        // Init resources of PhysX
        m_PhysX->Init();

        Info("PhysX initialized successfully!");
    }

    void PhysicsSystem::Update(Scene *scene, float deltaTime) {
    }

    void PhysicsSystem::Shutdown() {
        m_PhysX->Shutdown(); // Shutdown PhysX lifetime and low-level states like Physics, Foundation etc.
    }
}

/*
 *
*void VelocitySystem::Update(Scene *scene, float deltaTime) {
        const auto& view = scene->GetAllEntitiesWith<VelocityComponent, TransformComponent>();

        for (const auto& [entity, vc, tc] : view.each()) {
            auto& transform = tc.transform;

            vc.m_LinearVelocity = (
                transform.Right() * vc.m_Speed.x +
                transform.Up()    * vc.m_Speed.y +
                transform.Forward() * vc.m_Speed.z
            );

            // TODO: need movement system to update transform stuff
            transform.Translate(vc.m_LinearVelocity);

            // TODO: Add acceleration for rotation
        }
    }
 *
 */