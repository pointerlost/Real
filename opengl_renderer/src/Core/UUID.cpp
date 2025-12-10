//
// Created by pointerlost on 10/8/25.
//
#include "Core/UUID.h"
#include <sstream>
#include <random>

namespace Real {

    static std::random_device s_RandomDevice;
    static std::mt19937_64 s_Engine(s_RandomDevice());
    static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

    UUID::UUID() : m_UUID(s_UniformDistribution(s_Engine))
    {
    }

    UUID::UUID(uint64_t uuid) : m_UUID(uuid) {}
}
