//
// Created by pointerlost on 10/4/25.
//
#pragma once
#include <nlohmann/json.hpp>
#include "Utils.h"
#include "UUID.h"
#include "Common/RealEnum.h"
#include "Common/RealTypes.h"
#include "Graphics/RenderTypes.h"

namespace Real {
    class Shader;
    struct OpenGLTexture;
    struct Model;
    struct Material;
    struct MaterialInstance;
}
struct ImFont;

namespace Real {

    class AssetManager {
    public:
        AssetManager();

        /* *********************************** TEXTURE STATE ************************************ */
        void SaveTextureCPU(const Ref<OpenGLTexture> &tex);
        void LoadTextureArraysToGPU() const;
        [[maybe_unused]] Ref<OpenGLTexture>& GetOrCreateDefaultTexture(TextureType type);
        [[maybe_unused]] bool IsTextureCompressed(const String& stem) const;
        TextureData LoadTextureFromFile(const String& path, TextureType type = TextureType::UNDEFINED);
        void DeleteCPUTexture(const UUID& uuid);
        [[nodiscard]] const Ref<OpenGLTexture>& GetTexture(const UUID& uuid, TextureType type);
        Vector<Ref<OpenGLTexture>> GetMaterialTextures(const Material* mat);

        /* *********************************** MATERIAL STATE ************************************ */
        Ref<Material> CreateMaterialBase(const String& name);
        Ref<Material> GetMaterialBase(const String& assetName);
        Ref<Material> GetMaterialBase(const UUID& assetUUID);
        Ref<Material> LoadMaterialBaseAsset(const UUID& uuid, const String& name);
        Ref<Material> GetOrCreateMaterialBase(const String& name);
        [[nodiscard]] UUID CreateMaterialInstance(const UUID& assetUUID);
        [[nodiscard]] UUID CreateMaterialInstance(const String& assetName);
        [[nodiscard]] UUID GetMaterialAssetUUIDByName(const String& assetName);
        [[nodiscard]] Ref<MaterialInstance> GetMaterialInstance(const UUID& instanceUUID);

        /* *********************************** GENERAL STATE ************************************ */
        [[nodiscard]] Shader &GetShader(const String& name);
        bool IsModelExist(const String& name);
        Ref<Model> GetModel(const String& name);
        bool IsMaterialExist(const String& name);
        void RenameMaterial(const String& newName, const UUID& uuid); // Persistent renaming should inside AssetImporter
        void SaveModelCPU(const Ref<Model>& model);
        // Save base CPU material to Asset manager (not instance)
        void SaveMaterialCPU(const Ref<Material>& material);
        const std::unordered_map<UUID, Ref<Material>>& GetBaseMaterials();

        // TODO: Load fonts from file!!
        void AddFontStyle(const String& fontName, ImFont* font);
        ImFont* GetFontStyle(const String& fontName);

        /* ********************************** LOADING STATE ************************************ */
        void Update();
        void LoadShader(const String& vertexPath, const String& fragmentPath, const String& name);
        [[nodiscard]] Vector<graphics::BindlessHandle> UploadTexturesToGPU();

    private:
        std::unordered_map<String, Shader> m_Shaders; // TODO: Use UUIDs to store shaders??
        std::unordered_map<UUID, Ref<OpenGLTexture>> m_Textures;
        std::unordered_map<UUID, Ref<Material>> m_Materials;
        std::unordered_map<String, UUID> m_MaterialNameToUUID;
        std::unordered_map<UUID, Ref<MaterialInstance>> m_MaterialInstances;
        std::unordered_map<UUID, Ref<Model>> m_Models;
        std::unordered_map<String, UUID> m_ModelNameToUUID;
        std::unordered_map<TextureType, Ref<OpenGLTexture>> m_DefaultTextures;
        std::unordered_map<String, ImFont*> m_Fonts;

    private:
        void LoadDefaultTextures();
        String LoadShadersWithPreprocessor(const String& filePath);
        String GenerateUniqueMaterialName(const String& desiredName);
        String NormalizeMaterialName(String name);
    };
}
