//
// Created by pointerlost on 3/23/26.
//
#include "Physics/PhysicsUtils.h"
#include "PxPhysics.h"
#include "Core/Logger.h"

namespace Real::util::physics {

    physx::PxShape* CreatePhysXShapeFromReal(
        physx::PxPhysics& px,
        const physx::PxMaterial* mat,
        core::ShapeDesc::Shape shape)
    {
        switch (shape) {
            case core::ShapeDesc::Shape::Box:
                return px.createShape(physx::PxBoxGeometry(5.0f, 5.0f, 5.0f), *mat, true);

            case core::ShapeDesc::Shape::Capsule:
                return px.createShape(physx::PxCapsuleGeometry(0.5f, 1.0f), *mat, true);

            case core::ShapeDesc::Shape::Sphere:
                return px.createShape(physx::PxSphereGeometry(1.0f), *mat, true);

            default: {
                Info("No collider shape found, returning box!");
                return px.createShape(physx::PxBoxGeometry(5.0f, 5.0f, 5.0f), *mat, true);
            }
        }
    }

}
