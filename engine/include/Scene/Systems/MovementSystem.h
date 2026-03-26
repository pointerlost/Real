//
// Created by pointerlost on 1/25/26.
//
#pragma once
#include "Core/ISystem.h"

namespace Real::ecs {

    class MovementSystem final : public ISystem {
        void Init() override;
        void Update(entt::registry& registry, f32 deltaTime) override;
        void Shutdown() override;
    };
}
