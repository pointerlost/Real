//
// Created by pointerlost on 10/7/25.
//
#include "Input/Input.h"

#include "Graphics/Config.h"
#include "Input/CameraInput.h"

namespace Real::Input {

    double g_MouseX = 0.0;
    double g_MouseLastX = SCREEN_WIDTH / 2;
    double g_MouseY = 0.0;
    double g_MouseLastY = SCREEN_HEIGHT / 2;
    float g_Sensitivity = 0.079f;

    float g_Yaw = -90.0f;
    float g_Pitch = 0.0f;
    double g_Scroll = 0.0f;
    float g_ScrollSensitivity = 256.0;
    bool g_IsScrolling = false;

    bool g_KeyLastDown[REAL_KEY_COUNT] = { false };
    bool g_KeyCurrDown[REAL_KEY_COUNT] = { false };
    bool g_KeyNone[REAL_KEY_COUNT] = { true };
    bool g_KeyPressed[REAL_KEY_COUNT] = { false };
    bool g_KeyHeld[REAL_KEY_COUNT] = { false };
    bool g_KeyReleased[REAL_KEY_COUNT] = { false };

    bool IsScrolling() { return g_IsScrolling; }
    void ResetScroll() { g_Scroll = 0.0f; }

    float GetScroll() { return static_cast<float>(g_Scroll); }

    void Update(CameraInput* cameraInput) {

        for (int i = 0; i < REAL_KEY_COUNT; i++) {
            // Reset keyboard state
            g_KeyPressed[i] = false;
            g_KeyHeld[i] = false;
            g_KeyReleased[i] = false;

            if (g_KeyLastDown[i] && g_KeyCurrDown[i]) {
                g_KeyHeld[i] = true;
            }
            else if (g_KeyLastDown[i] && !g_KeyCurrDown[i]) {
                g_KeyReleased[i] = true;
            }
            else if (!g_KeyLastDown[i] && g_KeyCurrDown[i]) {
                g_KeyPressed[i] = true;
            } else {
                g_KeyNone[i] = true;
            }
            g_KeyLastDown[i] = g_KeyCurrDown[i];
        }

        // Update Camera movement
        cameraInput->Update();
    }

    bool IsKeyPressed(int key) {
        return g_KeyPressed[key];
    }

    bool IsKeyHeld(int key) {
        return g_KeyHeld[key];
    }

    bool IsKeyReleased(int key) {
        return g_KeyReleased[key];
    }

    bool IsKeyNone(int key) {
        return g_KeyNone[key];
    }

    void KeyDown(bool val, int key) {
        g_KeyCurrDown[key] = val;
    }

}
