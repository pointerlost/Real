//
// Created by pointerlost on 10/3/25.
//
#pragma once
#include <chrono>
#include "../Core/ITimer.h"

namespace Real {

    class RealTimeTimer final : public core::ITimer {
    public:
        void Start() override;
        void Update() override;
        void Stop() override;

        [[nodiscard]] f64 GetDelta() const override;
        [[nodiscard]] f64 GetElapsed() const override;
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
