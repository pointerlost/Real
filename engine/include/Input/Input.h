//
// Created by pointerlost on 10/7/25.
//
#pragma once
#include "Common/Types.h"

constexpr int REAL_KEY_COUNT = 349;
namespace Real { class CameraInput; }

namespace Real::Input {

    // Mouse movement
    extern f64 g_MouseX;
    extern f64 g_MouseLastX;
    extern f64 g_MouseY;
    extern f64 g_MouseLastY;
    extern f32 g_Sensitivity;

    // To calculate direction of camera from mouse yaw and pitch
    extern f32 g_Yaw;
    extern f32 g_Pitch;

    // Mouse scroll
    extern f64 g_Scroll;
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

    extern f32 GetScroll();
    extern void Update();

    extern bool IsKeyPressed(int key);
    extern bool IsKeyHeld(int key);
    extern bool IsKeyReleased(int key);
    extern bool IsKeyNone(int key);

    extern void KeyDown(bool val, int key);
}