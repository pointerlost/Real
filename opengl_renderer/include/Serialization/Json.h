//
// Created by pointerlost on 12/14/25.
//
#pragma once
#include <string>
#include <nlohmann/json.hpp>

namespace Real::serialization::json {
    void Save(const std::string& path, const nlohmann::json& j);
    nlohmann::json Load(const std::string& path);
}
