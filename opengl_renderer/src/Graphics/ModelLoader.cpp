//
// Created by pointerlost on 12/4/25.
//
#include <fstream>
#include <Graphics/ModelLoader.h>
#include <utility>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "Common/Macros.h"
#include "Core/AssetManager.h"
#include "Core/CmakeConfig.h"
#include "Core/Logger.h"
#include "Core/Services.h"
#include "Graphics/Material.h"
#include "Serialization/Binary.h"
#include "Tools/ImageTools.h"
#include "Util/Util.h"

namespace Real {

    void ModelLoader::LoadAll(const std::string &rootDir) {
        namespace std_fs = std::filesystem;

        auto IsModelFile = [](const std_fs::path& p) {
            if (!p.has_extension()) return false;
            const auto ext = p.extension().string();
            return ext == ".fbx" || ext == ".gltf" || ext == ".glb";
        };

        if (!std_fs::exists(rootDir)) {
            Warn("[ModelLoader::LoadAll] There is no rootDir for model loading!");
            return;
        }

        for (const auto& entry : std_fs::recursive_directory_iterator(rootDir)) {
            if (!entry.is_regular_file()) continue;

            const std::filesystem::path& modelPath = entry.path();
            if (!IsModelFile(modelPath)) {
                Info(ConcatStr(modelPath, " skipped!"));
                continue;
            }
            Load(modelPath.string(), ImageFormatState::COMPRESS_ME);
        }
    }

    Ref<Model> ModelLoader::Load(const std::string &filePath, ImageFormatState state) {
        if (!fs::File::Exists(filePath)) {
            Warn("Model file not found: " + filePath);
            return nullptr;
        }
        const auto& am = Services::GetAssetManager();
        const auto& mm = Services::GetMeshManager();

        // Reset state
        m_CurrentModel = CreateRef<Model>();
        m_CurrImageFormatState = state;
        m_CurrentModel->m_FileInfo = fs::CreateFileInfoFromPath(filePath);
        m_CurrentDirectory         = std::filesystem::path(m_CurrentModel->m_FileInfo.path).parent_path().string();
        m_CurrentModel->m_Name     = std::filesystem::path(m_CurrentDirectory).filename().string();

        // Create assimp importer
        Assimp::Importer importer;

        // flags for complex models
        constexpr unsigned int importFlags =
            aiProcess_Triangulate |
            aiProcess_GenSmoothNormals |
            aiProcess_FlipUVs |
            aiProcess_JoinIdenticalVertices |
            aiProcess_CalcTangentSpace |      // For normal mapping
            aiProcess_OptimizeMeshes |        // Important for large models
            aiProcess_ImproveCacheLocality;   // Better performance

        // Load the scene
        const aiScene* scene = importer.ReadFile(filePath, importFlags);

        // Check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            Warn(std::format("Failed to load model: {} - {}", filePath, importer.GetErrorString()));
            return nullptr;
        }

        // Start processing from root node
        ProcessNode(scene->mRootNode, scene, m_CurrentDirectory);

        const auto& vertices = mm->ViewVerticesPointToEnd(m_CurrentModel->m_Meshes[0].m_SubMeshID);
        const auto& indices  = mm->ViewIndicesPointToEnd(m_CurrentModel->m_Meshes[0].m_SubMeshID);
        const auto& binary_path = std::string(ASSETS_RUNTIME_DIR) + "models/" + m_CurrentModel->m_Name + ".model";
        serialization::binary::WriteModel(binary_path, m_CurrentBinaryFile, vertices, indices);
        am->SaveModelToAssetDB(m_CurrentModel);

        return m_CurrentModel;
    }

    void ModelLoader::ProcessNode(aiNode *node, const aiScene *scene, const std::string &directory) {
         // Process all meshes in this node
         for (unsigned int i = 0; i < node->mNumMeshes; i++) {
             aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
             MeshEntry meshEntry = ProcessMesh(mesh, scene, directory);
             m_CurrentModel->m_Meshes.push_back(meshEntry);
         }

         // Process all children nodes
         for (unsigned int i = 0; i < node->mNumChildren; i++) {
             ProcessNode(node->mChildren[i], scene, directory);
         }
     }

    MeshEntry ModelLoader::ProcessMesh(aiMesh *mesh, const aiScene *scene, const std::string &directory) {
         // Create containers for vertex data
         std::vector<Graphics::Vertex> vertices;
         std::vector<uint64_t> indices;

         // Process vertices
         vertices.reserve(mesh->mNumVertices);
         for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
             Graphics::Vertex vertex{};

             // Position
             vertex.m_Position.x = mesh->mVertices[i].x;
             vertex.m_Position.y = mesh->mVertices[i].y;
             vertex.m_Position.z = mesh->mVertices[i].z;

             // Normal
             if (mesh->HasNormals()) {
                 vertex.m_Normal.x = mesh->mNormals[i].x;
                 vertex.m_Normal.y = mesh->mNormals[i].y;
                 vertex.m_Normal.z = mesh->mNormals[i].z;
             } else {
                 vertex.m_Normal = glm::vec3(0.0, 1.0, 0.0);
             }

             // UV
             if (mesh->mTextureCoords[0]) {
                 vertex.m_UV.x = mesh->mTextureCoords[0][i].x;
                 vertex.m_UV.y = mesh->mTextureCoords[0][i].y;
             } else {
                 vertex.m_UV = glm::vec2(0.0, 0.0);
             }

             vertices.push_back(vertex);
         }

         // Process indices
         for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
             const aiFace face = mesh->mFaces[i];
             for (unsigned int j = 0; j < face.mNumIndices; j++) {
                 indices.push_back(face.mIndices[j]);
             }
         }

         // Process Material
         Ref<Material> real_material;
         if (mesh->mMaterialIndex >= 0) {
             const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
             real_material = ProcessMaterial(material, (int)mesh->mMaterialIndex, directory);
         }

         const auto meshID = Services::GetMeshManager()->CreateSingleMesh(vertices, indices, real_material->m_ID);

         return { meshID, real_material->m_ID };
    }

    Ref<Material> ModelLoader::ProcessMaterial(const aiMaterial *mat, int materialIndex, const std::string &directory) {
        const auto& am = Services::GetAssetManager();
        const auto material = CreateRef<Material>();

        // Material naming
        aiString matName;
        mat->Get(AI_MATKEY_NAME, matName);
        const auto baseName = matName.length > 0 ? matName.C_Str() : "Material_" + std::to_string(materialIndex);
        material->m_Name = m_CurrentModel->m_Name + "_" + baseName;

        std::string destPath = std::string(ASSETS_DIR) + "textures/";
        if (m_CurrImageFormatState == ImageFormatState::COMPRESS_ME)       destPath += "compress_me/";
        else if (m_CurrImageFormatState == ImageFormatState::UNCOMPRESSED) destPath += "uncompressed/";

        static std::unordered_map<TextureType, std::string_view> suffix {
            { TextureType::ALBEDO,            "_ALB"      },
            { TextureType::NORMAL,            "_NRM"      },
            { TextureType::ORM,               "_ORM"      },
            { TextureType::AMBIENT_OCCLUSION, "_AO"       },
            { TextureType::ROUGHNESS,         "_RGH"      },
            { TextureType::METALLIC,          "_MTL"      },
            { TextureType::HEIGHT,            "_HEIGHT"   },
            { TextureType::EMISSIVE,          "_EMISSIVE" }
            // Other special cases detecting in LoadTexture lambda
        };

        auto SaveTextureToFolder = [&](OpenGLTexture* tex, const std::string& dstPath) {
            if (!tex) {
                Warn("[ModelLoader][SaveTextureToFolder] Texture nullptr!");
                return;
            }
            if (!tools::SaveTextureAsFile(tex, dstPath)) {
                Warn(dstPath + " can't saved to destination path!");
                // TODO: add fallback
            }
        };

        auto CreateAndSave = [&](TextureData& td, const std::string& path, TextureType type) {
            FileInfo fi = fs::CreateFileInfoFromPath(path);
            const auto tex = CreateRef<OpenGLTexture>(td, type, ImageFormatState::COMPRESS_ME, fi);
            tools::CompressTextureAndReadFromFile(tex.get());
            SaveTextureToFolder(tex.get(), fi.path);
            am->SaveTextureToAssetDB(tex.get());
            return tex->GetUUID();
        };

        auto LoadTexture = [&](aiTextureType aiType) {
            if (mat->GetTextureCount(aiType) == 0) return;

            aiString texPath;
            aiTextureMapping mapping;
            unsigned int UVidx = 0;
            float blend = 1.0f;
            aiTextureOp op = aiTextureOp_Add;
            aiTextureMapMode mapMode = aiTextureMapMode_Wrap;

            if (mat->GetTexture(aiType, 0, &texPath, &mapping, &UVidx, &blend, &op, &mapMode) != AI_SUCCESS)
                return;

            const std::string filename = texPath.C_Str();
            const std::filesystem::path fullInputPath = std::filesystem::path(directory) / filename;

            const auto realType = util::AssimpTextureTypeToRealType(aiType);
            TextureData texData = am->LoadTextureFromFile(fullInputPath.string());
            texData.m_DataSize  = texData.m_Width * texData.m_Height * texData.m_ChannelCount;
            texData.m_Format    = util::ConvertChannelCountToGLFormat(texData.m_ChannelCount);
            texData.m_InternalFormat = util::ConvertChannelCountToGLInternalFormat(texData.m_ChannelCount);

            if (realType == TextureType::ALBEDO_ROUGHNESS) {
                auto alb = util::ExtractChannels(texData, {0,1,2});
                material->m_Albedo = CreateAndSave(alb,
                    destPath + m_CurrentModel->m_Name + "_ALB" + fullInputPath.extension().string(),
                    TextureType::ALBEDO
                );

                auto rgh = util::ExtractChannel(texData, 3);
                material->m_Roughness = CreateAndSave(rgh,
                    destPath + m_CurrentModel->m_Name + "_RGH" + fullInputPath.extension().string(),
                    TextureType::ROUGHNESS
                );
                return;
            }

            if (realType == TextureType::METALLIC_ROUGHNESS) {
                auto mtl = util::ExtractChannel(texData, 0);
                material->m_Metallic = CreateAndSave(mtl,
                    destPath + m_CurrentModel->m_Name + "_MTL" + fullInputPath.extension().string(),
                    TextureType::METALLIC
                );

                auto rgh = util::ExtractChannel(texData, 1);
                material->m_Roughness = CreateAndSave(rgh,
                    destPath + m_CurrentModel->m_Name + "_RGH" + fullInputPath.extension().string(),
                    TextureType::ROUGHNESS
                );
                return;
            }

            if (const auto it = suffix.find(realType); it != suffix.end()) {
                const auto outPath = destPath + m_CurrentModel->m_Name + std::string(it->second) + fullInputPath.extension().string();
                CreateAndSave(texData, outPath, it->first);
            }
        };

        // Check all the texture types from assimp and load
        for (int i = 0; i <= aiTextureType_GLTF_METALLIC_ROUGHNESS; i++) {
            const auto aiType = static_cast<aiTextureType>(i);
            if (aiType == aiTextureType_UNKNOWN || aiType == aiTextureType_NONE) continue;
            LoadTexture(aiType);
        }

        const auto& ao  = am->GetTexture(material->m_AO,        TextureType::AMBIENT_OCCLUSION);
        const auto& rgh = am->GetTexture(material->m_Roughness, TextureType::ROUGHNESS);
        const auto& mtl = am->GetTexture(material->m_Metallic,  TextureType::METALLIC);
        const auto& orm = tools::PackTexturesToRGBChannels({ao, rgh, mtl}, m_CurrentModel->m_Name);

        tools::CompressTextureAndReadFromFile(orm.get());
        material->m_ORM = orm->GetUUID();
        am->SaveTextureToAssetDB(orm.get());
        am->SaveMaterialToAssetDB(material);

        return material;
    }

    TextureType ModelLoader::GetRealTypeFromAssimpTexType(aiTextureType type) {
         switch (type) {
             case aiTextureType_DIFFUSE:
             case aiTextureType_BASE_COLOR:        return TextureType::ALBEDO;
             case aiTextureType_NORMAL_CAMERA:
             case aiTextureType_NORMALS:           return TextureType::NORMAL;
             case aiTextureType_DIFFUSE_ROUGHNESS: return TextureType::ALBEDO_ROUGHNESS;
             case aiTextureType_METALNESS:         return TextureType::METALLIC;
             case aiTextureType_AMBIENT:
             case aiTextureType_LIGHTMAP:
             case aiTextureType_AMBIENT_OCCLUSION: return TextureType::AMBIENT_OCCLUSION;
             case aiTextureType_OPACITY:           return TextureType::ALPHA;
             case aiTextureType_EMISSIVE:          return TextureType::EMISSIVE;
             case aiTextureType_HEIGHT:
             case aiTextureType_DISPLACEMENT:      return TextureType::HEIGHT;
             // TODO: Height and Displacement are different things in advanced pipeline, so should be different cases
             default: return TextureType::UNDEFINED;
         }
    }

}
