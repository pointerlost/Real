//
// Created by pointerlost on 1/14/26.
//
#include <Scene/Systems/PhysicsSystem.h>
#include "Core/Logger.h"
#include "Scene/Components.h"
#include "Scene/Scene.h"

namespace Real::ecs {
    using namespace physx;

    void PhysicsSystem::Init() {
        m_PhysX = CreateScope<PhysXContext>();
        // Init resources of PhysX
        m_PhysX->Init();

        Info("PhysX initialized successfully!");
    }

    void PhysicsSystem::Update(Scene *scene, float deltaTime) {
        const auto& view = scene->GetAllEntitiesWith<TransformComponent, VelocityComponent>();

        for (auto [entity, tc, vc] : view.each()) {
            auto& transform = tc.transform;

            // TODO: Replace speed-based movement with acceleration-based movement
            // For now, this is only used by the editor camera, so it's not a problem
            // When we move to in-game characters, this will need to change.
            vc.m_LinearVelocity = (
                transform.Right() * vc.m_Speed.x +
                transform.Up()    * vc.m_Speed.y +
                transform.Forward() * vc.m_Speed.z
            );

            // Integrate position
            tc.transform.Translate(vc.m_LinearVelocity * deltaTime);
        }
    }

    void PhysicsSystem::Shutdown() {
        m_PhysX->Shutdown(); // PhysX lifetime and low-level states like Physics, Foundation(PhysX) etc.
    }
}