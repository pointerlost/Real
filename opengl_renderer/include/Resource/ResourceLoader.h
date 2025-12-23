//
// Created by pointerlost on 12/6/25.
//
#pragma once
#include "Core/Utils.h"
#include "Graphics/ModelLoader.h"


namespace Real {
    class AssetImporter;
    class RenderContext;
}

namespace Real {

    class ResourceLoader {
    public:
        explicit ResourceLoader(RenderContext* context);

        void Load();

    private:
        RenderContext* m_RenderContext;
        Scope<ModelLoader> m_ModelLoader;

    private:
        void LoadAssets();
        void LoadShaders();
        void LoaderRenderContext();
    };

}
