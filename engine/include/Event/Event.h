//
// Created by pointerlost on 1/26/26.
//
#pragma once
#include <Common/RealTypes.h>
#include <functional>

namespace Real::event {

    template <typename... Args>
    class Event {
    public:
        using Callback = std::function<void(Args...)>;

        void Subscribe(Callback cb) {
            m_Listeners.push_back(std::move(cb));
        }

        void Emit(Args... args) {
            for (auto& listener : m_Listeners) {
                listener(args...);
            }
        }

    private:
        Vector<Callback> m_Listeners;
    };
}
