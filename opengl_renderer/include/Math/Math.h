//
// Created by pointerlost on 10/20/25.
//
#pragma once
#include <glm/glm.hpp>

namespace Real::Math {
    bool DecomposeTransform(const glm::mat4 &transform, glm::vec3& translation, glm::quat &rotation, glm::vec3 &scale);
}