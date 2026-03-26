//
// Created by pointerlost on 1/15/26.
//
#pragma once
#include "foundation/PxErrorCallback.h"

namespace Real::physics {

    class PhysXErrorCallback final : public physx::PxErrorCallback {
        void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) override;
    };
}
