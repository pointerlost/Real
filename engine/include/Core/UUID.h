//
// Created by pointerlost on 10/8/25.
//
#pragma once
#include <cstdint>
#include <string>

namespace Real {
    using u64 = uint64_t;
    using String = std::string;

    struct UUID {
        UUID();
        explicit UUID(u64 uuid);
        UUID(const UUID&) = default;

        [[nodiscard]] bool IsNull() const { return m_UUID == 0; };
        [[nodiscard]] String ToString() const;
        bool operator==(const UUID&) const = default;
        operator u64() const { return m_UUID; }

    private:
        u64 m_UUID = 0;
    };
}

namespace std {
    template <typename T> struct hash;

    template<>
    struct hash<Real::UUID> {
        std::size_t operator()(const Real::UUID& uuid) const {
            return (Real::u64)uuid;
        }
    };
}