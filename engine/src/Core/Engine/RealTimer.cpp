//
// Created by pointerlost on 10/3/25.
//
#include "../../../include/Core/Engine/RealTimer.h"
#include <chrono>

namespace Real {

    void RealTimer::Start() {
        m_Running = true;
        m_LastTime = Clock::now();
    }

    void RealTimer::Update() {
        if (!m_Running) return;

        const TimePoint now = Clock::now();

        const std::chrono::duration<f64> delta = now - m_LastTime;
        m_DeltaTime = delta.count();

        if (m_DeltaTime < 0.0)
            m_DeltaTime = 0.0;

        m_ElapsedTime += m_DeltaTime;

        m_LastTime = now;
    }

    void RealTimer::Stop() {
        m_Running = false;
    }

    f64 RealTimer::GetDelta() const {
        return m_DeltaTime;
    }

    f64 RealTimer::GetElapsed() const {
        return m_ElapsedTime;
    }

    int RealTimer::GetFPS() const {
        return m_DeltaTime > 0.0 ? static_cast<int>(1.0 / m_DeltaTime) : 0;
    }
}
