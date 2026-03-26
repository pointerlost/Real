//
// Created by pointerlost on 3/23/26.
//
#pragma once
#include <unordered_map>
#include "Common/Types.h"

namespace Real::graphics {
    class Shader;
    enum class ShaderType;
}

namespace Real::assets {

    class ShaderManager {
    public:
        void Load(const String& vertPath, const String& fragPath, const graphics::ShaderType& type);
        [[nodiscard]] graphics::Shader& Get(const graphics::ShaderType& type);
        [[nodiscard]] bool              Exists(const graphics::ShaderType& type) const;

    private:
        std::unordered_map<graphics::ShaderType, graphics::Shader> m_Shaders;

    private:
        [[nodiscard]] String PreprocessFile(const String& filePath) const;
    };
}
