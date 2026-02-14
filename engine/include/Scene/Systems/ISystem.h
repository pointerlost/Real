//
// Created by pointerlost on 2/14/26.
//
#pragma once
#include "Common/RealTypes.h"

namespace Real {
    class Scene;
}

namespace Real {

    struct ISystem {
        virtual ~ISystem() = default;
        virtual void Init() = 0;
        virtual void Update(Scene*, f32) = 0;
        virtual void Shutdown() = 0;
    };
}
