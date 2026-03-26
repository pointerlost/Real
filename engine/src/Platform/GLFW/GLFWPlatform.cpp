//
// Created by pointerlost on 2/17/26.
//
#include "Platform/GLFW/GLFWPlatform.h"
#include <stdexcept>
#include <GLFW/glfw3.h>

namespace Real::platform {

    void GLFWPlatform::Initialize() {
        if (!glfwInit())
            throw std::runtime_error("Failed to initialize GLFW");

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE,        GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT,  GL_TRUE);
    }

    void GLFWPlatform::Shutdown() {
        glfwTerminate();
    }
}
