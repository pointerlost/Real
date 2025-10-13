//
// Created by pointerlost on 10/3/25.
//
#pragma once
#include <cstdint>
#include <iostream>


namespace Real {

    class Timer {
    public:
        Timer();

        void Start();
        void Update();
        void Stop();

        [[nodiscard]] float GetDelta() const;
        [[nodiscard]] float GetElapsed() const;

    private:
        float m_CurrFrameTime = 0.0f;
        float m_DeltaTime = 0.0f;
        float m_LastFrameTime = 0.0f;
        float m_ElapsedTime = 0.0f;

        bool m_Running = false;
    };
}