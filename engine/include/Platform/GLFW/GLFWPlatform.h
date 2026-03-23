//
// Created by pointerlost on 2/17/26.
//
#pragma once
#include "Core/IPlatform.h"

namespace Real::platform {

    class GLFWPlatform final : public core::IPlatform {
    public:
        void Initialize() override;
        void Shutdown() override;
    };
}
