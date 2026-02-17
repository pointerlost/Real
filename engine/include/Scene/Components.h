//
// Created by pointerlost on 10/8/25.
//
#pragma once
#include <utility>
#include <vector>
#include "Core/Utils.h"
#include "Core/UUID.h"
#include "Graphics/Camera.h"
#include "Graphics/Light.h"
#include "Graphics/Transformations.h"
#include "Physics/PhysicsTypes.h"

namespace physx {
    class PxShape;
    class PxRigidActor;
}

namespace Real {
    struct Model;
    struct MaterialInstance;
}

namespace Real {
    // TODO: Add dirty flags to manage components and to avoid unnecessary updates

    struct TagComponent {
        String m_Tag{};

        TagComponent() = default;
        explicit TagComponent(String tag) : m_Tag(std::move(tag)) {}
        TagComponent(const TagComponent&) = default;
    };

    struct IDComponent {
        UUID m_UUID{};
        IDComponent() = default;
        explicit IDComponent(UUID uuid) : m_UUID(uuid) {}
        bool operator==(const IDComponent &) const = default;
    };

    struct TransformComponent {
        Transform transform{};
        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = delete;
    };

    struct MeshRendererComponent {
        Vector<UUID> m_MeshUUIDs = {};
        Vector<UUID> m_MaterialInstanceUUIDs = {};
        MeshRendererComponent(const Vector<UUID>& meshUUIDs, const Vector<UUID>& matInstanceUUIDs)
            : m_MeshUUIDs(meshUUIDs), m_MaterialInstanceUUIDs(matInstanceUUIDs) {}
        MeshRendererComponent(const UUID& meshUUID, const UUID& matInstanceUUID)
            : m_MeshUUIDs{meshUUID}, m_MaterialInstanceUUIDs{matInstanceUUID} {}
        MeshRendererComponent() = default;
        MeshRendererComponent(MeshRendererComponent&) = default;
    };

    // This component is only for behavior. It's optional and not necessary for PhysX
    struct RigidbodyComponent {
        physics::BodyType type = physics::BodyType::Static;
        f32 mass = 1.0f;

        physics::RigidBodyHandle handle = physics::InvalidRigidBodyHandle; // transient
    };

    struct ColliderComponent {
        physics::ColliderShape shape = physics::ColliderShape::Box;

        // Geometry
        math::Vec3 size{0.5f};
        // Local offset relative to actor
        math::Vec3 localPosition{0.0f};
        math::Quat localRotation = math::Quat::Identity();

        bool isTrigger = false;
        // User intent
        bool enabled = true; // true = Attach shape, false = Detach shape
        bool rebuildRequired = false;

        physics::PhysicsShapeHandle handle = physics::InvalidShapeHandle; // transient
    };

    struct MovementComponent {
        math::Vec3 moveInput = { 0.0, 0.0, 0.0 }; // normalized direction
        f32 maxSpeed = 6.0f;
        f32 acceleration = 20.0f;
        f32 airControl = 0.4f;

        bool jumpRequested  = false;
    };

    struct ModelComponent {
        Ref<Model> m_Model{};
        explicit ModelComponent(Ref<Model> model) : m_Model(std::move(model)) {}
        ModelComponent() = default;
        ModelComponent(const ModelComponent&) = default;
    };

    struct LightComponent {
        Light m_Light{};
        explicit LightComponent(const Light &light) : m_Light(light) {}
        explicit LightComponent(Light::Mode mode) : m_Light(Light{mode}) {}
        LightComponent() = default;
        LightComponent(const LightComponent&) = default;
    };

    struct CameraComponent {
        Camera m_Camera{};
        explicit CameraComponent(Camera camera) : m_Camera(std::move(camera)) {}
        CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;
    };
}
