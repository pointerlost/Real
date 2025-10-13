//
// Created by pointerlost on 10/8/25.
//
#pragma once
#include <utility>

#include "Core/Utils.h"
#include "Core/UUID.h"
#include "Graphics/Camera.h"
#include "Graphics/Light.h"
#include "Graphics/Transformations.h"

namespace Real {
    struct MaterialInstance;
}

namespace Real {

    struct TagComponent {
        std::string Tag;

        TagComponent() = default;
        explicit TagComponent(std::string tag) : Tag(std::move(tag)) {}
        TagComponent(const TagComponent&) = default;
    };

    struct IDComponent {
        explicit IDComponent(UUID uuid) : m_UUID(uuid) {}
        UUID m_UUID;
        bool operator==(const IDComponent &) const = default;
    };

    struct TransformComponent {
        Transformations m_Transform{};
        TransformComponent() = default;
    };

    struct MaterialComponent {
        Ref<MaterialInstance> m_Instance;
        explicit MaterialComponent(const Ref<MaterialInstance>& mat) : m_Instance(mat) {}
        MaterialComponent(MaterialComponent&) = default;
        MaterialComponent() = default;
    };

    struct MeshComponent {
        std::string m_MeshName;

        explicit MeshComponent(const std::string& meshName) : m_MeshName(meshName) {}
        MeshComponent() = default;
        MeshComponent(MeshComponent&) = default;
    };

    struct LightComponent {
        Light m_Light{};

        explicit LightComponent(const Light light) : m_Light(light) {}
        LightComponent() = default;
        LightComponent(const LightComponent&) = default;
    };

    struct CameraComponent {
        Camera m_Camera;

        explicit CameraComponent(Camera camera) : m_Camera(std::move(camera)) {}
        CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;
    };
}
