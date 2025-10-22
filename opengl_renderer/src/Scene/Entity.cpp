//
// Created by pointerlost on 10/8/25.
//
#include "Scene/Entity.h"
#include "Scene/Scene.h"

namespace Real {

    Entity::Entity(Scene *scene, entt::entity entity) : m_Handle(entity), m_Scene(scene)
    {
    }

}
