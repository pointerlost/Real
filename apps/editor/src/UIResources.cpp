//
// Created by pointerlost on 3/23/26.
//
#include "UIResources.h"

#include "Core/Logger.h"
#include "Core/Utils.h"

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

}
