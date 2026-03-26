//
// Created by pointerlost on 1/25/26.
//
#include <Scene/Systems/MovementSystem.h>
#include "Scene/Components.h"

namespace Real::ecs {

    void MovementSystem::Init() {
    }

    void MovementSystem::Update(entt::registry& registry, f32 deltaTime) {
        /*
         * MovementSystem JOB:
            - Non-physics movement (editor camera, debug objects)
        */
        const auto view = registry.view<TransformComponent, MovementComponent>(entt::exclude<RigidbodyComponent>);

        for (const auto entity : view) {
            auto& tc       = view.get<TransformComponent>(entity);
            const auto& mc = view.get<MovementComponent>(entity);

            // MovementSystem takes the normalized input(moveInput) and converts it to World-Space using Entity rotation
            math::Vec3 forward = tc.transform.Forward(); // includes pitch
            math::Vec3 right   = tc.transform.Right();
            math::Vec3 moveDir = forward * mc.moveInput.z + right * mc.moveInput.x;

            if (math::Vec3::LengthSq(moveDir) > 0.0f)
                moveDir = moveDir.Normalized();

            tc.transform.TranslateLocal(moveDir * mc.maxSpeed * deltaTime);
        }
    }

    void MovementSystem::Shutdown() {
    }

}
