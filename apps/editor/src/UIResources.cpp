//
// Created by pointerlost on 3/23/26.
//
#include "UIResources.h"

#include <imgui.h>

#include "Core/Logger.h"
#include "../../../engine/include/Common/Utils.h"
#include "Assets/FileManager.h"
#include "Core/CMakeConfig.h"

namespace Real::editor {

    void UIResources::AddFont(const String& name, ImFont* font) {
        if (!font) {
            Warn("Font is nullptr! name: " + name);
            return;
        }
        if (m_Fonts.contains(name)) {
            Warn("Font already exists: " + name);
            return;
        }
        m_Fonts[name] = font;
    }

    ImFont* UIResources::GetFont(const String& name) const {
        const auto it = m_Fonts.find(name);
        if (it == m_Fonts.end()) {
            Warn(ConcatStr("Font doesn't exist! name: ", name));
            return nullptr;
        }
        return it->second;
    }

    void UIResources::InitResources() {
        // Font style
        // Hardcoded for now!!
        const auto assets_dir = String(ASSETS_DIR);

        const ImGuiIO& io = ImGui::GetIO();
        if (const auto fontFile = assets_dir + "fonts/Ubuntu/Ubuntu-Regular.ttf"; fs::File::Exists(fontFile)) {
            AddFont("Ubuntu-Regular",
                io.Fonts->AddFontFromFileTTF(fontFile.c_str(),
                16.5f,
                nullptr,
                io.Fonts->GetGlyphRangesDefault())
            );
        }
        if (const auto fontFile = assets_dir + "fonts/Ubuntu/Ubuntu-Regular.ttf"; fs::File::Exists(fontFile)) {
            AddFont("Ubuntu-Regular-Big",
                io.Fonts->AddFontFromFileTTF(fontFile.c_str(),
                17.5f,
                nullptr,
                io.Fonts->GetGlyphRangesDefault())
            );
        }
        if (const auto fontFile = assets_dir + "fonts/Ubuntu/Ubuntu-Bold.ttf"; fs::File::Exists(fontFile)) {
            AddFont("Ubuntu-Bold",
                io.Fonts->AddFontFromFileTTF(fontFile.c_str(),
                16.5f,
                nullptr,
                io.Fonts->GetGlyphRangesDefault())
            );
        }
        if (const auto fontFile = assets_dir + "fonts/Ubuntu/Ubuntu-Bold.ttf"; fs::File::Exists(fontFile)) {
            AddFont("Ubuntu-Bold-Big",
                io.Fonts->AddFontFromFileTTF(fontFile.c_str(),
                17.5f,
                nullptr,
                io.Fonts->GetGlyphRangesDefault())
            );
        }
    }
}
