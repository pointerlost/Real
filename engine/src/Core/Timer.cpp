//
// Created by pointerlost on 10/3/25.
//
#include "Core/Timer.h"
#include <chrono>

namespace Real {

    void RealTimeTimer::Start() {
        m_Running = true;
        m_LastTime = Clock::now();
    }

    void RealTimeTimer::Update() {
        if (!m_Running) return;

        const TimePoint now = Clock::now();

        const std::chrono::duration<f64> delta = now - m_LastTime;
        m_DeltaTime = delta.count();

        if (m_DeltaTime < 0.0)
            m_DeltaTime = 0.0;

        m_ElapsedTime += m_DeltaTime;

        m_LastTime = now;
    }

    void RealTimeTimer::Stop() {
        m_Running = false;
    }

    f64 RealTimeTimer::GetDelta() const {
        return m_DeltaTime;
    }

    f64 RealTimeTimer::GetElapsed() const {
        return m_ElapsedTime;
    }

    int RealTimeTimer::GetFPS() const {
        return m_DeltaTime > 0.0 ? static_cast<int>(1.0 / m_DeltaTime) : 0;
    }
}
