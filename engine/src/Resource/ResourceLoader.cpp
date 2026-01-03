//
// Created by pointerlost on 12/7/25.
//
#include "Resource/ResourceLoader.h"

#include "Core/AssetImporter.h"
#include "Core/AssetManager.h"
#include "Core/Logger.h"
#include "Core/Services.h"
#include "Graphics/RenderContext.h"


namespace Real {

    ResourceLoader::ResourceLoader(RenderContext *context)
        : m_RenderContext(context), m_ModelLoader(CreateScope<ModelLoader>())
    {
    }

    void ResourceLoader::Load() {
        LoadAssets();
        LoadShaders();
        LoaderRenderContext();
    }

    void ResourceLoader::LoadAssets() {
        const auto& ai = Services::GetAssetImporter();
        const auto& mm = Services::GetMeshManager();
        // The order is matter!!
        ai->ImportFromDatabase();
        mm->LoadPrimitiveTypes();

        m_ModelLoader->LoadAll(std::string(ASSETS_SOURCE_DIR) + "models/");

        // If there are new assets from the ModelLoader, upload them to the database!
        ai->LoadNewAssetsToDataBase();

        // Load meshes after all the data processed
        mm->InitResources();

        m_RenderContext->GetGPURenderData().textures = Services::GetAssetManager()->UploadTexturesToGPU();

        Info("[ResourceLoader] Assets loaded successfully!");
    }

    void ResourceLoader::LoadShaders() {
        const auto& am = Services::GetAssetManager();

        const auto vert = ConcatStr(SHADERS_DIR, "opengl/main.vert");
        const auto frag = ConcatStr(SHADERS_DIR, "opengl/main.frag");
        am->LoadShader(vert, frag, "main");
        Info("[ResourceLoader] Shaders loaded successfully!");
    }

    void ResourceLoader::LoaderRenderContext() {
        m_RenderContext->InitResources();
    }
}
