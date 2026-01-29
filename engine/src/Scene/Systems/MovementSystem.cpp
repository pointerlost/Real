//
// Created by pointerlost on 1/25/26.
//
#include <Scene/Systems/MovementSystem.h>

#include "Scene/Components.h"
#include "Scene/Scene.h"

namespace Real::ecs {

    void MovementSystem::Init() {
    }

    void MovementSystem::Update(Scene *scene, float deltaTime) {
        /*
         * MovementSystem JOB:
            - Moves editor camera
            - Non-physics movement (editor camera, debug objects)
        */
        const auto view = m_Registry->view<TransformComponent, MovementComponent>(entt::exclude<PhysicsBodyComponent>);

        for (const auto entity : view) {
            auto& tc = view.get<TransformComponent>(entity);
            const auto& mc = view.get<MovementComponent>(entity);

            // MovementSystem takes the normalized input(moveInput) and converts it to World-Space using Entity rotation
            math::Vec3 forward = tc.transform.Forward(); // includes pitch
            math::Vec3 right   = tc.transform.Right();
            math::Vec3 moveDir = forward * mc.moveInput.z + right * mc.moveInput.x;

            if (math::Vec3::LengthSq(moveDir) > 0.0f)
                moveDir = moveDir.Normalized();

            tc.transform.Translate(moveDir * mc.maxSpeed * deltaTime);
        }
    }

    void MovementSystem::Shutdown() {
    }

    void MovementSystem::SetRegistry(entt::registry &registry) {
        m_Registry = &registry;
    }
}
