//
// Created by pointerlost on 10/24/25.
//
#pragma once
#include "Scene/Systems.h"

namespace Real {

    class TransformUpdate final : public Systems {
        void Update(Scene *scene, float deltaTime) override;
    };

    class VelocityUpdate final : public  Systems {
        void Update(Scene *scene, float deltaTime) override;
    };

    class MeshRendererUpdate final : public Systems {
        void Update(Scene *scene, float deltaTime) override;
    };

    class CameraUpdate final : public Systems {
        void Update(Scene *scene, float deltaTime) override;
    };

    class LightUpdate final : public Systems {
        void Update(Scene *scene, float deltaTime) override;
    };
}