//
// Created by pointerlost on 2/17/26.
//
#pragma once
#include "Common/RealTypes.h"

namespace Real::core {

    class ITimer {
    public:
        virtual ~ITimer() = default;

        virtual void Start() = 0;
        virtual void Stop() = 0;
        virtual void Update() = 0;

        [[nodiscard]] virtual f64 GetDelta() const = 0;
        [[nodiscard]] virtual f64 GetElapsed() const = 0;
    };
}