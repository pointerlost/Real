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
    // TODO: Add dirty flags to manage components and avoid to unnecessary updates

    struct TagComponent {
        std::string m_Tag{};

        TagComponent() = default;
        explicit TagComponent(std::string tag) : m_Tag(std::move(tag)) {}
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
        std::vector<UUID> m_MeshUUIDs = {};
        std::vector<UUID> m_MaterialInstanceUUIDs = {};
        MeshRendererComponent(const std::vector<UUID>& meshUUIDs, const std::vector<UUID>& matInstanceUUIDs)
            : m_MeshUUIDs(meshUUIDs), m_MaterialInstanceUUIDs(matInstanceUUIDs) {}
        MeshRendererComponent(const UUID& meshUUID, const UUID& matInstanceUUID)
            : m_MeshUUIDs{meshUUID}, m_MaterialInstanceUUIDs{matInstanceUUID} {}
        MeshRendererComponent() = default;
        MeshRendererComponent(MeshRendererComponent&) = default;
    };

    // This component is only for behavior. It's optional and not necessary for PhysX
    struct PhysicsBodyComponent {
        physics::BodyType bodyType = physics::BodyType::Static;
        float mass = 1.0f;

        PhysicsBodyComponent() = default;
        explicit PhysicsBodyComponent(const physics::BodyType bodyType, float mass = 1.0f)
            : bodyType(bodyType), mass(mass) {}
    };

    struct ColliderComponent {
        physics::ColliderShape shape = physics::ColliderShape::Box;

        // Geometry
        math::Vec3 size{ 0.5f };

        // Local offset relative to actor
        math::Vec3 localPosition{ 0.0f };
        math::Quat localRotation = math::Quat::Identity();

        bool isTrigger = false;

        // User intent
        bool enabled = true; // true = Attach shape, false = Detach shape

        // Runtime state (PhysX truth)
        bool attached = false;

        // Runtime (physics)
        physx::PxRigidActor* actor = nullptr;
        physx::PxShape* shapeHandle = nullptr;

        // Editor-only debug mode
        physics::ColliderDebug debug;
    };

    struct MovementComponent {
        math::Vec3 moveInput = { 0.0, 0.0, 0.0 }; // normalized direction
        float maxSpeed = 6.0f;
        float acceleration = 20.0f;
        float airControl = 0.4f;

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
        explicit LightComponent(const LightType type) : m_Light(Light{type}) {}
        LightComponent() = default;
        LightComponent(const LightComponent&) = default;
    };

    struct CameraComponent {
        Camera m_Camera{CameraMode::Perspective};
        explicit CameraComponent(Camera camera) : m_Camera(std::move(camera)) {}
        CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;
    };
}
