//
// Created by pointerlost on 2/14/26.
//
#pragma once
#include "Common/Types.h"
#include "entt/entt.hpp"

namespace Real {

    struct ISystem {
        virtual ~ISystem() = default;
        virtual void Init() = 0;
        virtual void Update(entt::registry& /*registry(entt)*/, f32 /*delta time*/) = 0;
        virtual void Shutdown() = 0;
    };
}
