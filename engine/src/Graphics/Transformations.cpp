//
// Created by pointerlost on 10/9/25.
//
#include "Graphics/Transformations.h"

#include <algorithm>

namespace Real {

    Transform::Transform(Transform &&other) noexcept
        : m_CachedWorldMatrix(other.m_CachedWorldMatrix)
        , m_Dirty(other.m_Dirty)
        , m_LocalPosition(other.m_LocalPosition)
        , m_LocalScale(other.m_LocalScale)
        , m_LocalRotation(other.m_LocalRotation)
        , m_Parent(other.m_Parent)
        , m_Children(std::move(other.m_Children))
    {
        // Fix up children - they still point to other, redirect to this
        for (auto* child : m_Children)
            child->m_Parent = this;

        // Fix up parent - replace other with this in parent's list
        if (m_Parent) {
            auto& siblings = m_Parent->m_Children;
            std::ranges::replace(siblings, &other, this);
        }

        // Leave other in a valid empty state
        other.m_Parent = nullptr;
        other.m_Children.clear();
    }
}
