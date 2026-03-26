//
// Created by pointerlost on 3/23/26.
//
#pragma once
#include "Core/IPhysicsBackend.h"

namespace physx {
    class PxMaterial;
    class PxPhysics;
    class PxShape;
}

namespace Real::util::physics {

    physx::PxShape* CreatePhysXShapeFromReal(
        physx::PxPhysics& px,
        const physx::PxMaterial* mat,
        core::ShapeDesc::Shape shape
    );
}
