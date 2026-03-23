//
// Created by pointerlost on 12/7/25.
//
#include "../../include/Assets/ResourceLoader.h"
#include "Common/Macros.h"
#include "../../include/Assets/AssetImporter.h"
#include "../../include/Assets/AssetManager.h"
#include "Core/Logger.h"
#include "Core/Services.h"
#include "Graphics/MeshManager.h"
#include "Graphics/RenderContext.h"


namespace Real {

    ResourceLoader::ResourceLoader()
        : m_ModelLoader(CreateScope<ModelLoader>())
    {
    }

    void ResourceLoader::Load() {
        LoadAssets();
        LoadShaders();
    }

    void ResourceLoader::LoadAssets() {
        const auto& ai = Services::GetAssetImporter();
        const auto& mm = Services::GetMeshManager();
        // The order is matter!!

        // Asset importer state
        ai->ImportFromDatabase();

        // Model loader state
        m_ModelLoader->LoadAll(String(ASSETS_SOURCE_DIR) + "models/");

        // Mesh manager state
        mm->InitResources();

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

}
