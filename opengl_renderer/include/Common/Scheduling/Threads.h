//
// Created by pointerlost on 11/4/25.
//
#pragma once
#include <functional>
#include <thread>
#include <vector>

namespace Real {

    class Thread {
    public:
        template <typename T, typename... Args>
        void Submit(T type, Args... arg) {
            workers.emplace_back(type, arg...);
        }

        void JoinAll() {
            for (auto&& worker : workers) {
                if (worker.joinable()) {
                    worker.join();
                }
            }
        }

    private:
        std::vector<std::thread> workers;
    };
}
