//
// Created by pointerlost on 12/7/25.
//
#include "Assets/ResourceLoader.h"
#include "Common/Macros.h"
#include "Assets/AssetImporter.h"
#include "Assets/MeshManager.h"
#include "Assets/ModelManager.h"
#include "Assets/ShaderManager.h"
#include "Assets/TextureManager.h"
#include "Core/Logger.h"
#include "Core/Services.h"
#include "Core/Engine/EngineCore.h"
#include "Graphics/RenderContext.h"

namespace Real::assets {

    ResourceManager::ResourceManager()
    {
    }

    void ResourceManager::Load(core::CoreSystems* core) {
        LoadAssets(core);
        LoadShaders();
    }

    void ResourceManager::LoadAssets(core::CoreSystems* core) const {
        Info("Loading assets...");
        auto& ai     = Services::GetAssetImporter();
        auto& meshM  = Services::GetMeshManager();
        auto& modelM = Services::GetModelManager();
        auto& texM   = Services::GetTextureManager();

        // The asset loading order is matter!!

        // Asset importer state
        ai.ImportFromDatabase();
        Info("Assets loaded from ASSET_DB successfully!");

        // Model loader state
        modelM.LoadAll(String(ASSETS_SOURCE_DIR) + "models/");
        Info("Models loaded from folder successfully!");

        // Mesh manager state
        meshM.InitResources();
        Info("Mesh manager init resources successfully!");

        core->renderer->GetRenderContext().GetGPURenderData().textures = texM.FlushPendingUploads();

        Info("[ResourceLoader] Assets loaded successfully!");
    }

    void ResourceManager::LoadShaders() {
        // TODO: I need to save shaders to asset database!
        auto& sm = Services::GetShaderManager();

        auto vert = ConcatStr(SHADERS_DIR, "opengl/main.vert");
        auto frag = ConcatStr(SHADERS_DIR, "opengl/main.frag");
        sm.Load(vert, frag, ShaderType::Main);

        // Load shader and save to AssetManager then get and use
        vert = ConcatStr(SHADERS_DIR, "opengl/debug/main.vert");
        frag = ConcatStr(SHADERS_DIR, "opengl/debug/main.frag");
        sm.Load(vert, frag, ShaderType::Debug);

        Info("[ResourceLoader] Shaders loaded successfully!");
    }

}
