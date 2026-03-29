//
// Created by pointerlost on 12/6/25.
//
#pragma once
#include "Common/Utils.h"
#include "Graphics/ModelLoader.h"

namespace Real::assets {

    class ResourceManager {
    public:
        ResourceManager();

        void Load();

    private:
        Scope<graphics::ModelLoader> m_ModelLoader;

    private:
        void LoadAssets() const;
        void LoadShaders();
    };

}
