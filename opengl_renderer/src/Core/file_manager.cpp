//
// Created by pointerlost on 9/7/25.
//
#include "Core/file_manager.h"
#include <fstream>
#include "Core/Logger.h"

using namespace std;

namespace Real {

    std::string File::ReadFromFile(const std::string &path) {
        if (!Exists(path)) {
            Warn("Path doesn't exists: " + path);
            return {};
        }

        std::string content;
        ifstream stream(path, std::ios::in);
        if (!stream.is_open()) {
            Warn("File can't opening!");
            return {};
        }

        std::string line;
        while (getline(stream, line)) {
            content.append(line + "\n");
        }

        stream.close();
        return content;
    }

    bool File::Exists(const std::string &path) {
        return std::filesystem::exists(path);
    }
}
