//
// Created by pointerlost on 10/3/25.
//
#include "Core/Timer.h"

#include <GLFW/glfw3.h>

namespace Real {

    Timer::Timer() {
    }

    void Timer::Start() {
        m_Running = true;
    }

    void Timer::Update() {
        if (!m_Running) return;

        // Get current time with glfw
        m_CurrFrameTime = static_cast<float>(glfwGetTime());

        m_DeltaTime = m_CurrFrameTime - m_LastFrameTime;

        // The time since the application began
        m_ElapsedTime += m_DeltaTime;

        m_LastFrameTime = m_CurrFrameTime;
    }

    void Timer::Stop() {
        m_Running = false;
    }

    float Timer::GetDelta() const {
        return m_DeltaTime;
    }

    float Timer::GetElapsed() const {
        return m_ElapsedTime;
    }
}
