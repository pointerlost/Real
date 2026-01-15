//
// Created by pointerlost on 1/13/26.
//
#pragma once
#include "Systems.h"
#include "Physics/PhysXContext.h"
#include "Core/Utils.h"

namespace Real::ecs {

    // This class manages: REAL engine logic and ECS integration (with PhysX)
    class PhysicsSystem final : public Systems {
    public:
        void Init() override;
        void Update(Scene* scene, float deltaTime) override;
        void Shutdown() override;

    private:
        Scope<PhysXContext> m_PhysX;
    };

}
