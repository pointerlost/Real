//
// Created by pointerlost on 10/3/25.
//
#include "Core/Timer.h"
#include <chrono>

namespace Real {

    Timer::Timer() {
    }

    void Timer::Start() {
        m_Running = true;
    }

    void Timer::Update() {
        if (!m_Running) return;

        const TimePoint now = Clock::now();

        std::chrono::duration<f64> delta = now - m_LastTime;
        m_DeltaTime = delta.count();

        if (m_DeltaTime < 0.0)
            m_DeltaTime = 0.0;

        m_ElapsedTime += m_DeltaTime;

        m_LastTime = now;
    }

    void Timer::Stop() {
        m_Running = false;
    }

    f64 Timer::GetDelta() const {
        return m_DeltaTime;
    }

    f64 Timer::GetElapsed() const {
        return m_ElapsedTime;
    }

    int Timer::GetFPS() const {
        return static_cast<int>(1.0 / m_DeltaTime);
    }
}
