//
// Created by pointerlost on 1/13/26.
//
#pragma once
#include "PxShape.h"

namespace physx {
    class PxShape;
    class PxRigidActor;
}

namespace Real::physics {

    struct PhysXBody {
        physx::PxRigidActor* actor;
    };

    struct PhysXShape {
        physx::PxShape* shape;
    };
}
