//
// Created by pointerlost on 3/23/26.
//
#include "Assets/ShaderManager.h"
#include "Graphics/Shader.h"
#include <fstream>
#include <functional>
#include <unordered_set>
#include "Core/CMakeConfig.h"
#include "Core/Logger.h"
#include "Core/Utils.h"

namespace Real {

    void ShaderManager::Load(const String& vertPath, const String& fragPath, const String& name) {
        const auto vert = PreprocessFile(vertPath);
        const auto frag = PreprocessFile(fragPath);
        m_Shaders[name] = Shader{vert, frag, name};
    }

    Shader& ShaderManager::Get(const String& name) {
        if (!m_Shaders.contains(name))
            Warn(ConcatStr("Shader doesn't exist! from: ", __FILE__));
        return m_Shaders.at(name);
    }

    bool ShaderManager::Exists(const String& name) const {
        return m_Shaders.contains(name);
    }

    String ShaderManager::PreprocessFile(const String& filePath) const {
        std::unordered_set<String> includedFiles;
        String output;
        bool versionWritten = false;

        std::function<void(const String&)> processFile = [&](const String& path) {
            if (includedFiles.contains(path)) return;
            includedFiles.insert(path);

            std::ifstream file(path);
            if (!file.is_open()) {
                Warn("Shader include failed: " + path);
                return;
            }

            String line;
            while (std::getline(file, line)) {
                if (line.starts_with("#version")) {
                    if (!versionWritten) {
                        output += line + "\n";
                        versionWritten = true;
                    }
                    continue;
                }
                if (line.starts_with("#include")) {
                    const size_t firstQuote = line.find('"');
                    const size_t lastQuote  = line.find_last_of('"');
                    if (firstQuote == String::npos || lastQuote <= firstQuote) continue;
                    processFile(SHADERS_DIR + line.substr(firstQuote + 1, lastQuote - firstQuote - 1));
                    continue;
                }
                output += line + "\n";
            }
        };

        processFile(filePath);
        return output;
    }

}
