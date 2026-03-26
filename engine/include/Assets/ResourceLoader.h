//
// Created by pointerlost on 12/6/25.
//
#pragma once
#include "../Common/Utils.h"
#include "Graphics/ModelLoader.h"

namespace Real {
    class RenderContext;
}

namespace Real::assets {

    class ResourceManager {
    public:
        ResourceManager();

        void Load();

    private:
        Scope<ModelLoader> m_ModelLoader;

    private:
        void LoadAssets();
        void LoadShaders();
    };

}
