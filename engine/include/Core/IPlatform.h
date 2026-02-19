//
// Created by pointerlost on 2/17/26.
//
#pragma once

namespace Real::core {

    class IPlatform {
    public:
        virtual ~IPlatform() = default;
        virtual void Initialize() = 0;
        virtual void Shutdown() = 0;
    };
}