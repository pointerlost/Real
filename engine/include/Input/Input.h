//
// Created by pointerlost on 10/7/25.
//
#pragma once

constexpr int REAL_KEY_COUNT = 349;
namespace Real { class CameraInput; }

namespace Real::Input {

    // Mouse movement
    extern double g_MouseX;
    extern double g_MouseLastX;
    extern double g_MouseY;
    extern double g_MouseLastY;
    extern float g_Sensitivity;

    // To calculate direction of camera from mouse yaw and pitch
    extern float g_Yaw;
    extern float g_Pitch;

    // Mouse scroll
    extern double g_Scroll;
    extern bool g_IsScrolling;

    extern bool IsScrolling();
    extern void ResetScroll();

    // Key press state
    extern bool g_KeyLastDown[REAL_KEY_COUNT];
    extern bool g_KeyCurrDown[REAL_KEY_COUNT];
    extern bool g_KeyNone[REAL_KEY_COUNT];
    extern bool g_KeyPressed[REAL_KEY_COUNT];
    extern bool g_KeyHeld[REAL_KEY_COUNT];
    extern bool g_KeyReleased[REAL_KEY_COUNT];

    extern float GetScroll();
    extern void Update(CameraInput* cameraInput);

    extern bool IsKeyPressed(int key);
    extern bool IsKeyHeld(int key);
    extern bool IsKeyReleased(int key);
    extern bool IsKeyNone(int key);

    extern void KeyDown(bool val, int key);
}