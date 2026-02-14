//
// Created by pointerlost on 1/14/26.
//
#pragma once
#include "ISystem.h"

namespace Real::ecs {

    class CameraSystem final : public ISystem {
        void Init() override;
        void Update(Scene *scene, f32 deltaTime) override;
        void Shutdown() override;
    };
}
