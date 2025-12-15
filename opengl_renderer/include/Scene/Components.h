//
// Created by pointerlost on 10/8/25.
//
#pragma once
#include <vector>
#include <utility>
#include "Core/Utils.h"
#include "Core/UUID.h"
#include "Graphics/Camera.h"
#include "Graphics/Light.h"
#include "Graphics/Transformations.h"

namespace Real {
    struct Model;
    struct MaterialInstance;
}

namespace Real {
    // TODO: Add dirty flags to manage components and avoid to unnecessary updates

    struct TagComponent {
        std::string m_Tag;

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
        Transformations m_Transform{};
        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = delete;
    };

    struct VelocityComponent {
        glm::vec3 m_LinearVelocity = glm::vec3(0.0);
        glm::vec3 m_Acceleration   = glm::vec3(0.0);
        glm::vec3 m_Speed          = glm::vec3(0.0);

        VelocityComponent() = default;
        VelocityComponent(const VelocityComponent&) = default;
    };

    struct MeshRendererComponent {
        UUID m_MeshID = {};
        std::vector<UUID> m_MaterialUUIDs;
        MeshRendererComponent(const UUID& meshUUID, const std::vector<UUID>& matUUIDs)
            : m_MeshID(meshUUID), m_MaterialUUIDs(matUUIDs) {}
        MeshRendererComponent(const UUID& meshUUID, const UUID& matUUID)
            : m_MeshID(meshUUID), m_MaterialUUIDs(matUUID) {}
        MeshRendererComponent() = default;
        MeshRendererComponent(MeshRendererComponent&) = default;
    };

    struct MaterialOverrideComponent {
        UUID m_UUID;
        explicit MaterialOverrideComponent(const UUID& uuid) : m_UUID(uuid) {}
        MaterialOverrideComponent() = default;
        MaterialOverrideComponent(const MaterialOverrideComponent&) = default;
    };

    struct ModelComponent {
        Ref<Model> m_Model;
        explicit ModelComponent(Ref<Model> model) : m_Model(std::move(model)) {}
        ModelComponent() = default;
        ModelComponent(const ModelComponent&) = default;
    };

    struct LightComponent {
        Light m_Light{};
        explicit LightComponent(const Light &light) : m_Light(light) {}
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
