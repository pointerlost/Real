#pragma once
#include "Scene/Scene.h"

namespace Real {

    template <typename T>
    T& Entity::AddComponent() {
        assert(!HasComponent<T>());
        T& component = m_Scene->GetRegistry().template emplace<T>(m_Handle);
        m_Scene->template OnComponentAdded<T>(*this, component);
        return component;
    }

    template <typename T, typename... Args>
    T& Entity::AddComponent(Args&&... args) {
        assert(!HasComponent<T>());
        T& component = m_Scene->GetRegistry().template emplace<T>(m_Handle, std::forward<Args>(args)...);
        m_Scene->template OnComponentAdded<T>(*this, component);
        return component;
    }

    // variadic multi-add
    template <typename... Args>
    void Entity::AddComponents() {
        (AddComponent<Args>(), ...);
    }

    template <typename T>
    void Entity::RemoveComponent() {
        if (HasComponent<T>()) {
            m_Scene->GetRegistry().template erase<T>(m_Handle);
        }
    }

    // the safe throwing version
    template <typename T>
    T& Entity::GetComponent() {
        auto* component = TryGetComponent<T>();
        if (!component) {
            throw std::runtime_error("Component doesn't exist: " + String(typeid(T).name()));
        }
        return *component;
    }

    template <typename T>
    T* Entity::TryGetComponent() const noexcept {
        return m_Scene->GetRegistry().template try_get<T>(m_Handle);
    }

    template <typename T>
    bool Entity::HasComponent() const noexcept {
        return m_Scene->GetRegistry().template any_of<T>(m_Handle);
    }

    template <typename... Args>
    bool Entity::HasComponents() const noexcept {
        return m_Scene->GetRegistry().template all_of<Args...>(m_Handle);
    }

    template <typename T>
    T& Entity::GetComponentUnchecked() const noexcept {
        return m_Scene->GetRegistry().template get<T>(m_Handle);
    }

    template<typename T>
    T* Entity::GetComponentForModification() {
        return TryGetComponent<T>();
    }

}
