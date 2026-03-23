//
// Created by pointerlost on 3/23/26.
//
#pragma once
#include <unordered_map>
#include "Common/Types.h"

namespace Real { class Shader; }

namespace Real {

    class ShaderManager {
    public:
        void                  Load(const String& vertPath, const String& fragPath, const String& name);
        [[nodiscard]] Shader& Get(const String& name);
        [[nodiscard]] bool    Exists(const String& name) const;

    private:
        std::unordered_map<String, Shader> m_Shaders;

    private:
        [[nodiscard]] String PreprocessFile(const String& filePath) const;
    };
}
