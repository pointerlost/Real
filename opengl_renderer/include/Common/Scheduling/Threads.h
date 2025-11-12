//
// Created by pointerlost on 11/4/25.
//
#pragma once
#include <thread>
#include <vector>

namespace Real {

    class Thread {
    public:
        template <typename T, typename... Args>
        void Submit(T&& type, Args&&... arg) {
            m_Workers.emplace_back(std::forward<T>(type), std::forward<Args>(arg)...);
        }

        void JoinAll() {
            for (auto&& worker : m_Workers) {
                if (worker.joinable()) {
                    worker.join();
                }
            }
        }

    private:
        std::vector<std::thread> m_Workers;
    };
}
