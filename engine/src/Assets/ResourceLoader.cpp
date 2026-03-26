//
// Created by pointerlost on 12/7/25.
//
#include "Assets/ResourceLoader.h"
#include "Common/Macros.h"
#include "Assets/AssetImporter.h"
#include "Assets/MeshManager.h"
#include "Assets/ShaderManager.h"
#include "Core/Logger.h"
#include "Core/Services.h"

namespace Real::assets {

    ResourceManager::ResourceManager()
        : m_ModelLoader(CreateScope<ModelLoader>())
    {
    }

    void ResourceManager::Load() {
        LoadAssets();
        LoadShaders();
    }

    void ResourceManager::LoadAssets() {
        auto& ai = Services::GetAssetImporter();
        auto& mm = Services::GetMeshManager();
        // The order is matter!!

        // Asset importer state
        ai.ImportFromDatabase();

        // Model loader state
        m_ModelLoader->LoadAll(String(ASSETS_SOURCE_DIR) + "models/");

        // Mesh manager state
        mm.InitResources();

        Info("[ResourceLoader] Assets loaded successfully!");
    }

    void ResourceManager::LoadShaders() {
        // TODO: I need to save shaders to asset database!
        auto& sm = Services::GetShaderManager();

        auto vert = ConcatStr(SHADERS_DIR, "opengl/main.vert");
        auto frag = ConcatStr(SHADERS_DIR, "opengl/main.frag");
        sm.Load(vert, frag, graphics::ShaderType::Main);

        // Load shader and save to AssetManager then get and use
        vert = ConcatStr(SHADERS_DIR, "opengl/debug/main.vert");
        frag = ConcatStr(SHADERS_DIR, "opengl/debug/main.frag");
        sm.Load(vert, frag, graphics::ShaderType::Debug);

        Info("[ResourceLoader] Shaders loaded successfully!");
    }

}
