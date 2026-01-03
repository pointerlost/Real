//
// Created by pointerlost on 10/8/25.
//
#pragma once
#include <Scene/Scene.h>
#include <entt/entt.hpp>
#include "Core/Logger.h"
#include "Core/Utils.h"

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
        operator uint32_t() const {
            return static_cast<uint32_t>(m_Handle);
        }
        [[nodiscard]] bool IsExists() const { return m_Handle != entt::null; }

        template <typename T>
        [[nodiscard]] T& AddComponent() {
            assert(!HasComponent<T>() && "Component already exists!");

            T& component = m_Scene->GetRegistry().emplace<T>(m_Handle);
            m_Scene->OnComponentAdded<T>(*this, component);
            return component;
        }

        template <typename T, typename... Args>
        [[nodiscard]] T& AddComponent(Args&&... args) {
            assert(!HasComponent<T>() && "Component already exists!");

            T& component = m_Scene->GetRegistry().emplace<T>(m_Handle, std::forward<Args>(args)...);
            m_Scene->OnComponentAdded<T>(*this, component);
            return component;
        }

        // Add multiple components at once
        template <typename... Args>
        void AddComponents() {
            (AddComponent<Args>(), ...);  // Will assert if any already exists
        }

        template <typename T>
        void RemoveComponent() {
            if (HasComponent<T>()) {
                m_Scene->GetRegistry().erase<T>(m_Handle);
            }
        }

        // TWO VERSIONS: One safe, one unsafe
        template<typename T>
        [[nodiscard]] T* TryGetComponent() const noexcept {
            return m_Scene->GetRegistry().try_get<T>(m_Handle);
        }

        template<typename T>
        [[nodiscard]] T& GetComponent() {
            auto* component = TryGetComponent<T>();
            if (!component) {
                throw std::runtime_error("Component doesn't exist: " + std::string(typeid(T).name()));
            }
            return *component;
        }

        // For when we know the component exists and want maximum performance
        template<typename T>
        [[nodiscard]] T& GetComponentUnchecked() const noexcept {
            return m_Scene->GetRegistry().get<T>(m_Handle);
        }

        // Non-const version for modification
        template<typename T>
        [[nodiscard]] T* GetComponentForModification() {
            return TryGetComponent<T>();
        }

        template <typename T>
        [[nodiscard]] bool HasComponent() const noexcept {
            return m_Scene->GetRegistry().any_of<T>(m_Handle);
        }

        template <typename... Args>
        [[nodiscard]] bool HasComponents() const noexcept {
            return m_Scene->GetRegistry().all_of<Args...>(m_Handle);
        }

    private:
        entt::entity m_Handle;
        Scene* m_Scene;
    };
}
