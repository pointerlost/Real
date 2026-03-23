//
// Created by pointerlost on 10/8/25.
//
#pragma once
#include "Common/Types.h"
#include "entt/entt.hpp"

namespace Real { class Scene; }

namespace Real {

    class Entity {
    public:
        Entity(Scene* scene, entt::entity entity);
        Entity() = default;
        Entity(const Entity&) = default;

        bool operator==(const Entity& e) const {
            return m_Handle == e.m_Handle;
        }
        operator entt::entity() const {
            return m_Handle;
        }
        operator u32() const {
            return static_cast<u32>(m_Handle);
        }
        [[nodiscard]] bool IsExists() const { return m_Handle != entt::null; }

        template <typename T>
        [[nodiscard]] T& AddComponent();

        template <typename T, typename... Args>
        [[nodiscard]] T& AddComponent(Args&&... args);

        // Add multiple components at once
        template <typename... Args>
        void AddComponents();

        template <typename T>
        void RemoveComponent();

        // TWO VERSIONS: One safe, one unsafe
        template<typename T>
        [[nodiscard]] T* TryGetComponent() const noexcept;

        template<typename T>
        [[nodiscard]] T& GetComponent();

        // For when we know the component exists and want maximum performance
        template<typename T>
        [[nodiscard]] T& GetComponentUnchecked() const noexcept;

        // Non-const version for modification
        template<typename T>
        [[nodiscard]] T* GetComponentForModification();

        template <typename T>
        [[nodiscard]] bool HasComponent() const noexcept;

        template <typename... Args>
        [[nodiscard]] bool HasComponents() const noexcept;

        [[nodiscard]] Scene& GetScene() const   { return *m_Scene; }
        [[nodiscard]] entt::entity& GetHandle() { return m_Handle; }

    private:
        entt::entity m_Handle;
        Scene* m_Scene;
    };
}

#include "Scene/Entity.inl"