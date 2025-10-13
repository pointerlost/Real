//
// Created by pointerlost on 10/8/25.
//
#pragma once
#include <cstdint>
#include <string>

namespace Real {

    struct UUID {
        UUID();
        UUID(const UUID&) = default;

        bool operator==(const UUID&) const = default;
        operator uint64_t() const { return m_UUID; }

    private:
        uint64_t m_UUID;
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