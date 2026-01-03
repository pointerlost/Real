//
// Created by pointerlost on 11/4/25.
//
#pragma once
#include <thread>
#include <vector>

namespace Real {

    // Basic multitasking class for now...
    class Thread {
    public:
        explicit Thread(int workerCount = 16) : m_WorkerCount(workerCount) {}

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

        [[nodiscard]] bool IsWorkerAvailable() const { return m_WorkerCount > m_Workers.size(); }

    private:
        std::vector<std::thread> m_Workers{};
        // std::condition_variable m_CV;
        int m_WorkerCount = 0;
    };
}
