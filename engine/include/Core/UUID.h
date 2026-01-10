//
// Created by pointerlost on 10/8/25.
//
#pragma once
#include <cstdint>
#include <string>

namespace Real {

    struct UUID {
        UUID();
        explicit UUID(uint64_t uuid);
        UUID(const UUID&) = default;

        [[nodiscard]] bool IsNull() const { return m_UUID == 0; };
        [[nodiscard]] std::string ToString() const { return std::to_string(m_UUID); }
        bool operator==(const UUID&) const = default;
        operator uint64_t() const { return m_UUID; }

    private:
        uint64_t m_UUID = 0;
    };
}

namespace std {

    template <typename T> struct hash;

    template<>
    struct hash<Real::UUID> {
        std::size_t operator()(const Real::UUID& uuid) const {
            return (uint64_t)uuid;
        }
    };
}