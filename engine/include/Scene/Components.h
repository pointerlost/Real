//
// Created by pointerlost on 10/8/25.
//
#pragma once
#include "Common/Utils.h"
#include "Core/IPhysicsBackend.h"
#include "Graphics/Camera.h"
#include "Graphics/Light.h"
#include "Graphics/Transformations.h"
#include "Core/UUID.h"

namespace Real::graphics { struct Model; }

namespace Real {
    // TODO: Add dirty flags to manage components and to avoid unnecessary updates

    struct TagComponent {
        std::string tag{};

        explicit TagComponent(std::string tag) : tag(std::move(tag)) {}
        TagComponent()                             = default;
        TagComponent(const TagComponent&) noexcept = default;
    };

    struct IDComponent {
        UUID id{};

        explicit IDComponent(const UUID uuid) : id(uuid) {}
        IDComponent()                              = default;
        bool operator==(const IDComponent &) const = default;
    };

    struct TransformComponent {
        Transform transform{};

        TransformComponent()                              = default;
        TransformComponent(const TransformComponent&)     = delete; // Transform is non-copyable
        TransformComponent(TransformComponent&&) noexcept = default;
    };

    struct MeshRendererComponent {
        Vector<UUID> meshIDs        = {};
        Vector<UUID> matInstanceIDs = {};

        MeshRendererComponent(const Vector<UUID>& meshID,const Vector<UUID>& matInstanceID)
            : meshIDs(meshID), matInstanceIDs(matInstanceID) {}
        MeshRendererComponent(const UUID& meshID, const UUID& matInstanceID)
            : meshIDs{meshID}, matInstanceIDs{matInstanceID} {}
        MeshRendererComponent()                                      = default;
        MeshRendererComponent(const MeshRendererComponent&) noexcept = default;
    };

    struct RigidbodyComponent {
        core::BodyDesc        desc   = {};
        core::RigidBodyHandle handle = {}; // transient
    };

    struct ColliderComponent {
        core::ShapeDesc   desc   = {}; // transient
        core::ShapeHandle handle = {};
    };

    struct MovementComponent {
        // Input
        math::Vec3 moveInput      = { 0.0, 0.0, 0.0 }; // normalized direction
        bool       jumpRequested  = false;

        // Tuning
        f32 maxSpeed     = 6.f;
        f32 acceleration = 20.f;
        f32 airControl   = 0.4f;
    };

    struct ModelComponent {
        Ref<graphics::Model> model{};

        explicit ModelComponent(Ref<graphics::Model> model) : model(std::move(model)) {}
                 ModelComponent()                      = default;
                 ModelComponent(const ModelComponent&) = default;
    };

    struct LightComponent {
        Light light{};

        explicit LightComponent(const Light &l)   : light(l)           {}
        explicit LightComponent(Light::Mode mode) : light(Light{mode}) {}
                 LightComponent()                      = default;
                 LightComponent(const LightComponent&) = default;
    };

    struct CameraComponent {
        Camera camera{};

        explicit CameraComponent(Camera cam) : camera(std::move(cam)) {}
                 CameraComponent()                       = default;
                 CameraComponent(const CameraComponent&) = default;
    };
}
