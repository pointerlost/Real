//
// Created by pointerlost on 1/15/26.
//
#include "Physics/PhysX/PhysXErrorCallback.h"
#include "Core/Logger.h"

namespace Real::physics {

    void PhysXErrorCallback::reportError(physx::PxErrorCode::Enum code, const char *message,
        const char *file, int line)
    {
        Error(message, file, line);
    }
}
