//
// Created by pointerlost on 2/12/26.
//
#pragma once
#include <cstdint>
#include "Graphics/Transformations.h"
#include "Math/Vec3.h"

namespace Real::core {

    struct RigidBodyHandle {
        uint32_t id = 0;
        bool operator==(const RigidBodyHandle&) const = default;
    };
    struct ShapeHandle {
        uint32_t id = 0;
        bool operator==(const ShapeHandle&) const = default;
    };

    // Invalid constants
    constexpr RigidBodyHandle InvalidBodyID { 0 };
    constexpr ShapeHandle     InvalidShapeID{ 0 };

    struct PhysicsWorldDesc {
        math::Vec3 gravity { 0.f, -9.81f, 0.f };
    };

    // Some physics libraries needs local-space thing,
    // So LocalPose provide a local transform to store Real stuff in local-space for physics libraries
    struct LocalPose {
        math::Vec3 position { 0.f };
        math::Quat rotation = math::Quat::Identity();
    };

    struct BodyDesc {
        enum class Type { Static, Dynamic, Kinematic };

        Type  type = Type::Static;
        float mass = 1.f;

        LocalPose localTransform;
    };

    struct ShapeDesc {
        enum class Shape { Box, Sphere, Capsule };

        // Geometry
        Shape      shape = Shape::Box;
        math::Vec3 size  { 0.5f };

        // Local offset relative to actor
        LocalPose localTransform;

        bool isTrigger       = false;
        bool enabled         = true; /*(user intent)*/ // true = Attach shape, false = Detach shape
        bool rebuildRequired = false;
    };

    class IPhysicsBackend {
    public:
        virtual ~IPhysicsBackend() = default;
        virtual void Init(const PhysicsWorldDesc&) = 0;
        virtual void Shutdown()                    = 0;
        virtual void Step(float dt)                = 0;

        [[nodiscard]] virtual RigidBodyHandle CreateBody(const BodyDesc&)   = 0;
        virtual void                          DestroyBody(RigidBodyHandle)  = 0;
        [[nodiscard]] virtual ShapeHandle     CreateShape(const ShapeDesc&) = 0;
        virtual void                          DestroyShape(ShapeHandle)     = 0;

        virtual void  AttachShape(RigidBodyHandle, ShapeHandle)                 = 0;
        virtual void  DetachShape(RigidBodyHandle, ShapeHandle)                 = 0;
        virtual void  SetBodyTransform(RigidBodyHandle, const LocalPose&)       = 0;
        [[nodiscard]] virtual LocalPose GetBodyTransform(RigidBodyHandle) const = 0;
    };
}

namespace std {
    template<>
    struct hash<Real::core::RigidBodyHandle> {
        size_t operator()(const Real::core::RigidBodyHandle& h) const noexcept {
            return hash<uint32_t>()(h.id);
        }
    };

    template<>
    struct hash<Real::core::ShapeHandle> {
        size_t operator()(const Real::core::ShapeHandle& h) const noexcept {
            return hash<uint32_t>()(h.id);
        }
    };
}