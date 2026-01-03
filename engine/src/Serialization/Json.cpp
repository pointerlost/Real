//
// Created by pointerlost on 12/14/25.
//
#include "Serialization/Json.h"

#include <fstream>

#include "Core/Logger.h"

namespace Real::serialization::json {

    void Save(const std::string &path, const nlohmann::json &j) {
        const std::string content = j.dump(4); // Serialize first

        std::ofstream file(path, std::ios::out | std::ios::binary);
        if (!file.is_open()) {
            Warn("[SaveJSON] Failed to open file: " + path);
            return;
        }

        file.write(content.data(), content.size());
        file.flush();
    }

    nlohmann::json Load(const std::string &path) {
        std::ifstream file(path);

        // File does not exist
        if (!file.is_open()) {
            return nlohmann::json::object();
        }

        // File exists but is empty
        if (file.peek() == std::ifstream::traits_type::eof()) {
            return nlohmann::json::object();
        }

        try {
            nlohmann::json j;
            file >> j;
            return j;
        }
        catch (const nlohmann::json::parse_error& e) {
            Warn("[LoadJSON] JSON parse error in " + path + ", " + e.what());
            return nlohmann::json::object();
        }
    }
}
