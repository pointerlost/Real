//
// Created by pointerlost on 10/4/25.
//
#pragma once
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <nlohmann/json.hpp>
#include "UUID.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"

namespace Real {
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
        [[maybe_unused]] bool IsTextureCompressed(const std::string& stem) const;
        TextureData LoadTextureFromFile(const std::string& path, TextureType type = TextureType::UNDEFINED);
        void DeleteCPUTexture(const UUID& uuid);
        [[nodiscard]] const Ref<OpenGLTexture>& GetTexture(const UUID& uuid, TextureType type);
        std::vector<Ref<OpenGLTexture>> GetMaterialTextures(const Material* mat);

        /* *********************************** MATERIAL STATE ************************************ */
        Ref<Material> CreateMaterialBase(const std::string& name);
        Ref<Material> GetMaterialBase(const std::string& assetName);
        Ref<Material> GetMaterialBase(const UUID& assetUUID);
        Ref<Material> LoadMaterialBaseAsset(const UUID& uuid, const std::string& name);
        Ref<Material> GetOrCreateMaterialBase(const std::string& name);
        [[nodiscard]] UUID CreateMaterialInstance(const UUID& assetUUID);
        [[nodiscard]] UUID CreateMaterialInstance(const std::string& assetName);
        [[nodiscard]] UUID GetMaterialAssetUUIDByName(const std::string& assetName);
        [[nodiscard]] Ref<MaterialInstance> GetMaterialInstance(const UUID& instanceUUID);

        /* *********************************** GENERAL STATE ************************************ */
        [[nodiscard]] const Shader &GetShader(const std::string& name);
        bool IsModelExist(const std::string& name);
        Ref<Model> GetModel(const std::string& name);
        bool IsMaterialExist(const std::string& name);
        void RenameMaterial(const std::string& newName, const UUID& uuid); // Persistent renaming should inside AssetImporter
        void SaveModelCPU(const Ref<Model>& model);
        // Save base CPU material to Asset manager (not instance)
        void SaveMaterialCPU(const Ref<Material>& material);
        const std::unordered_map<UUID, Ref<Material>>& GetBaseMaterials();

        // TODO: Load fonts from file!!
        void AddFontStyle(const std::string& fontName, ImFont* font);
        ImFont* GetFontStyle(const std::string& fontName);

        /* ********************************** LOADING STATE ************************************ */
        void Update();
        void LoadShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& name);
        [[nodiscard]] std::vector<GLuint64> UploadTexturesToGPU();

    private:
        std::unordered_map<std::string, Shader> m_Shaders; // TODO: Use UUIDs to store shaders??
        std::unordered_map<UUID, Ref<OpenGLTexture>> m_Textures;
        std::unordered_map<UUID, Ref<Material>> m_Materials;
        std::unordered_map<std::string, UUID> m_MaterialNameToUUID;
        std::unordered_map<UUID, Ref<MaterialInstance>> m_MaterialInstances;
        std::unordered_map<UUID, Ref<Model>> m_Models;
        std::unordered_map<std::string, UUID> m_ModelNameToUUID;
        std::unordered_map<TextureType, Ref<OpenGLTexture>> m_DefaultTextures;
        std::unordered_map<std::string, ImFont*> m_Fonts;

    private:
        void LoadDefaultTextures();
        std::string PreprocessorForShaders(const std::string& filePath);
        std::string GenerateUniqueMaterialName(const std::string& desiredName);
        std::string NormalizeMaterialName(std::string name);
    };
}
