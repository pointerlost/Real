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
            return m_Scene->GetRegistry().emplace<T>(m_Handle);
        }

        /* Callable for known component types */
        template <typename... Args>
        void AddComponents() {
            // TODO: Need test check if exists already before added because will cause crash!!
            (m_Scene->GetRegistry().emplace<Args>(m_Handle), ...);
        }

        template <typename Type>
        void RemoveComponent() {
            m_Scene->GetRegistry().erase<Type>(m_Handle);
        }

        template<typename Type>
        [[nodiscard]] Type* GetComponent() const {
            if (!HasComponent<Type>()) {
                Warn(ConcatStr(typeid(Type).name(), "doesn't exists!"));
                return nullptr;
            }
            return &m_Scene->GetRegistry().get<Type>(m_Handle);
        }

        template <typename Type>
        [[nodiscard]] bool HasComponent() const {
            return m_Scene->GetRegistry().any_of<Type>(m_Handle);
        }

        template <typename... Args>
        [[nodiscard]] bool HasComponents() const {
            return m_Scene->GetRegistry().all_of<Args...>(m_Handle);
        }

    private:
        entt::entity m_Handle;
        Scene* m_Scene;
    };
}
