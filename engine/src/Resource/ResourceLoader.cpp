//
// Created by pointerlost on 12/7/25.
//
#include "Resource/ResourceLoader.h"
#include "Common/Macros.h"
#include "Core/AssetImporter.h"
#include "Core/AssetManager.h"
#include "Core/Logger.h"
#include "Core/Services.h"
#include "Graphics/MeshManager.h"
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

        m_ModelLoader->LoadAll(String(ASSETS_SOURCE_DIR) + "models/");

        // If there are new assets from the ModelLoader, upload them to the database!
        ai->LoadNewAssetsToDataBase();

        // Load meshes after all the data processed
        mm->InitResources();

        m_RenderContext->GetGPURenderData().textures = Services::GetAssetManager()->UploadTexturesToGPU();

        Info("[ResourceLoader] Assets loaded successfully!");
    }

    void ResourceLoader::LoadShaders() {
        // TODO: I need to save shaders to asset database!
        const auto& am = Services::GetAssetManager();

        auto vert = ConcatStr(SHADERS_DIR, "opengl/main.vert");
        auto frag = ConcatStr(SHADERS_DIR, "opengl/main.frag");
        am->LoadShader(vert, frag, "main");

        // Load shader and save to AssetManager then get and use
        vert = ConcatStr(SHADERS_DIR, "opengl/debug/main.vert");
        frag = ConcatStr(SHADERS_DIR, "opengl/debug/main.frag");
        am->LoadShader(vert, frag, "debug");

        Info("[ResourceLoader] Shaders loaded successfully!");
    }

    void ResourceLoader::LoaderRenderContext() {
        m_RenderContext->InitResources();
    }
}
