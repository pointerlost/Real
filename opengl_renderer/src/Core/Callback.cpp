//
// Created by pointerlost on 10/7/25.
//
#include "Core/Callback.h"

#include "Core/Services.h"
#include "Editor/EditorState.h"
#include "Graphics/Config.h"
#include "Input/Input.h"
#include "Input/Keycodes.h"

namespace Real {

    void set_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (action == GLFW_PRESS) {
            Input::KeyDown(true, key);
        }
        if (action == GLFW_RELEASE) {
            Input::KeyDown(false, key);
        }
    }

    void set_mouse_callback(GLFWwindow *window, double xpos, double ypos) {
        if (s_FirstMouse) {
            Input::g_MouseLastX = xpos;
            Input::g_MouseLastY = ypos;
            s_FirstMouse = false;
        }
        Input::g_MouseLastX = Input::g_MouseX;
        Input::g_MouseLastY = Input::g_MouseY;
        Input::g_MouseX = xpos;
        Input::g_MouseY = ypos;

        if (!Services::GetEditorState()->fpsMode) return;
        if (Input::IsKeyHeld(REAL_MOUSE_BUTTON_LEFT) || Input::IsKeyPressed(REAL_MOUSE_BUTTON_LEFT)) {
            auto deltaX = Input::g_MouseX - Input::g_MouseLastX;
            auto deltaY = Input::g_MouseLastY - Input::g_MouseY; // Y-axis should be reversed

            deltaX *= Input::g_Sensitivity;
            deltaY *= Input::g_Sensitivity;

            Input::g_Yaw += static_cast<float>(deltaX);
            Input::g_Pitch += static_cast<float>(deltaY);

            if (Input::g_Pitch > 89.0f)  Input::g_Pitch = 89.0f;
            if (Input::g_Pitch < -89.0f) Input::g_Pitch = -89.0f;
        }
    }

    void set_mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
        if (button == REAL_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            Input::KeyDown(true, button);
        }
        if (action == GLFW_RELEASE) {
            Input::KeyDown(false, button);
        }
    }

    void set_mouse_scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
        Input::g_IsScrolling = true;
        Input::g_Scroll -= yoffset * Input::g_ScrollSensitivity;
    }

    void set_framebuffer_size_callback(GLFWwindow *window, int width, int height) {
        glViewport(0, 0, width, height);
    }

    void InitCallbacks(GLFWwindow* window) {
        glfwSetKeyCallback(window, set_key_callback);
        glfwSetFramebufferSizeCallback(window, set_framebuffer_size_callback);
        glfwSetCursorPosCallback(window, set_mouse_callback);
        glfwSetMouseButtonCallback(window, set_mouse_button_callback);
        glfwSetScrollCallback(window, set_mouse_scroll_callback);
        // Set GLFW_CURSOR_DISABLED for first-person shooter camera
        // in normal case = GLFW_CURSOR_NORMAL
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}
