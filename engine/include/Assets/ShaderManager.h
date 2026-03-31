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
    using namespace graphics;

    class ShaderManager final {
    public:
        void               Load(const String& vertPath, const String& fragPath, const ShaderType& type);
        [[nodiscard]] bool Exists(const ShaderType& type) const;
        [[nodiscard]] Shader& Get(const ShaderType& type);

    private:
        std::unordered_map<ShaderType, Shader> m_Shaders;

    private:
        [[nodiscard]] String PreprocessFile(const String& filePath) const;
    };
}
