//
// Created by pointerlost on 10/3/25.
//
#pragma once
#include <chrono>
#include "Common/RealTypes.h"

namespace Real {

    class Timer {
    public:
        Timer();

        void Start();
        void Update();
        void Stop();

        [[nodiscard]] f64 GetDelta() const;
        [[nodiscard]] f64 GetElapsed() const;
        [[nodiscard]] int GetFPS() const;

    private:
        using Clock = std::chrono::_V2::steady_clock;
        using TimePoint = std::chrono::time_point<Clock>;

        TimePoint m_LastTime{};

        f64 m_DeltaTime = 0.0f;
        f64 m_ElapsedTime = 0.0f;

        bool m_Running = false;
    };
}
