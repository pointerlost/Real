//
// Created by pointerlost on 3/23/26.
//
#pragma once
#include <unordered_map>
#include "Common/Types.h"

struct ImFont;

namespace Real::editor {

    class UIResources {
    public:
        void    AddFont(const String& name, ImFont* font);
        ImFont* GetFont(const String& name) const;

        void InitResources();

    private:
        std::unordered_map<String, ImFont*> m_Fonts;
    };

}
