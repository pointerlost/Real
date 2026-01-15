//
// Created by pointerlost on 1/14/26.
//
#pragma once
#include "Systems.h"

namespace Real::ecs {

    class TransformSystem final : public Systems {
        void Init() override;
        void Update(Scene *scene, float deltaTime) override;
        void Shutdown() override;
    };

}
