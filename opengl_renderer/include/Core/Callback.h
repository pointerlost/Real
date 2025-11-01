//
// Created by pointerlost on 10/7/25.
//
#pragma once
#include "compressonator/include/cmp_compressonatorlib/compressonator.h"
#include "Core/Window.h"

namespace Real {

    void set_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void set_mouse_callback(GLFWwindow* window, double xpos, double ypos);
    void set_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    void set_mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    void set_framebuffer_size_callback(GLFWwindow* window, int width, int height);

    void InitCallbacks(GLFWwindow* window);

    bool inline static s_FirstMouse = true;
}
