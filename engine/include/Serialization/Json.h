//
// Created by pointerlost on 12/14/25.
//
#pragma once
#include <nlohmann/json.hpp>
#include "Common/RealTypes.h"

namespace Real::serialization::json {
    void Save(const String& path, const nlohmann::json& j);
    nlohmann::json Load(const String& path);
}
