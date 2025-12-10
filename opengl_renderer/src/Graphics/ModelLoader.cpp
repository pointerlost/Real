//
// Created by pointerlost on 12/4/25.
//
#include <fstream>
#include <Graphics/ModelLoader.h>

#include <utility>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "Common/Macros.h"
#include "Common/RealTypes.h"
#include "Core/AssetManager.h"
#include "Core/CmakeConfig.h"
#include "Core/Logger.h"
#include "Core/Services.h"
#include "Graphics/Material.h"
#include "Tools/ImageTools.h"
#include "Util/Util.h"

namespace Real {

    void ModelLoader::LoadAll(const std::string &dir_path) {
        namespace fs = std::filesystem;

        auto IsModelFile = [&](const fs::path& path) {
            if (!path.has_extension()) return false;
            const std::string ext = path.extension().string();
            return ext == ".fbx" || ext == ".gltf" || ext == ".glb";
        };

        for (auto& entry : fs::directory_iterator(dir_path)) {
            if (!entry.is_directory()) continue;

            for (auto& file : fs::directory_iterator(entry.path())) {
                if (!file.is_regular_file()) continue;

                const auto& path = file.path();
                if (IsModelFile(path)) {
                    Load(path);
                    break;
                }
            }
        }
    }

     Ref<Model> ModelLoader::Load(const std::string &filePath, ImageFormatState state) {
         if (File::Exists(std::string(ASSETS_DIR) + std::filesystem::path(filePath).stem().string() + ".model")) {
             // TODO: load from binary file!
         }

         if (!File::Exists(filePath)) {
             Warn("Model file not found: " + filePath);
             return nullptr;
         }

         // Reset state
         m_CurrentModel = CreateRef<Model>();
         m_CurrImageFormatState = state;
         m_CurrentModel->m_FileInfo = util::CreateFileInfoFromPath(filePath);

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

         // TODO: Add material UUID and save to asset_database.json
         const auto meshID = Services::GetMeshManager()->CreateSingleMesh(vertices, indices);

         // Process Material
         Ref<Material> real_material;
         if (mesh->mMaterialIndex >= 0) {
             aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
             real_material = ProcessMaterial(material, directory);
         }

         return {  meshID, real_material->m_ID };
    }

    Ref<Material> ModelLoader::ProcessMaterial(const aiMaterial *mat, const std::string &directory) {
         const auto& am = Services::GetAssetManager();
         const auto material = CreateRef<Material>();
         const auto materialName = m_CurrentModel->m_FileInfo.stem;

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
         };

         auto SaveTextureToFolder = [&](OpenGLTexture* tex, const std::string& dstPath) {
             if (!tools::SaveTextureAsFile(tex, dstPath)) {
                 Warn(dstPath + " can't saved to destination path!");
                 // TODO: add fallback
             }
         };

         auto CreateAndSave = [&](TextureData& td, const std::string& path) {
             FileInfo fi = util::CreateFileInfoFromPath(path);
             const auto tex = CreateRef<OpenGLTexture>(td, fi);
             SaveTextureToFolder(tex.get(), fi.path);
             return tex->GetUUID();
         };

         auto LoadTexture = [&](aiTextureType aiType)
         {
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

             if (realType == TextureType::ALBEDO_ROUGHNESS) {
                 auto alb = util::ExtractChannels(texData, {0,1,2});
                 material->m_Albedo = CreateAndSave(alb, destPath + materialName + "_ALB" + fullInputPath.extension().string());

                 auto rgh = util::ExtractChannel(texData, 3);
                 material->m_Roughness = CreateAndSave(rgh, destPath + materialName + "_RGH" + fullInputPath.extension().string());
                 return;
             }

             if (realType == TextureType::METALLIC_ROUGHNESS) {
                 auto mtl = util::ExtractChannel(texData, 0);
                 material->m_Metallic = CreateAndSave(mtl, destPath + materialName + "_MTL" + fullInputPath.extension().string());

                 auto rgh = util::ExtractChannel(texData, 1);
                 material->m_Roughness = CreateAndSave(rgh, destPath + materialName + "_RGH" + fullInputPath.extension().string());
                 return;
             }

             if (const auto it = suffix.find(realType); it != suffix.end()) {
                 const auto outPath = destPath + materialName + std::string(it->second) + fullInputPath.extension().string();
                 CreateAndSave(texData, outPath);
             }
         };

         // Check all the texture types from assimp and load
         for (int i = 0; i < aiTextureType_UNKNOWN; i++) {
             const auto aiType = static_cast<aiTextureType>(i);
             LoadTexture(aiType);
         }

         const auto& ao  = am->GetTexture(material->m_AO);
         const auto& rgh = am->GetTexture(material->m_Roughness);
         const auto& mtl = am->GetTexture(material->m_Metallic);
         material->m_ORM = tools::PackTexturesToRGBChannels({ao, rgh, mtl}, materialName)->GetUUID();

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

    ModelBinaryHeader ModelLoader::LoadFromBinaryFile(const std::string &path) {
         ModelBinaryHeader model;
         std::vector<Graphics::Vertex> vertices;
         std::vector<uint64_t> indices;

         std::fstream file(path, std::ios::binary);
         if (!file) {
             Warn("There is no file, path: " + path);
             return{};
         }

         // Read REAL magic numbers
         file.read(reinterpret_cast<char*>(&model.m_Magic), 4);
         if (model.m_Magic != MakeFourCC('R', 'E', 'A', 'L')) { // Little-endian
             Warn("Real magic number mismatch!");
             return{};
         }

         // Read mesh count
         file.read((char*)&model.m_MeshCount, sizeof(uint64_t));
         model.m_MeshData.resize(model.m_MeshCount);

         // Read Version
         file.read(reinterpret_cast<char*>(&model.m_Version), sizeof(uint32_t));

         // Read per mesh data
         file.read(reinterpret_cast<char*>(model.m_MeshData.data()), model.m_MeshCount * sizeof(MeshEntry));

         // Read Mesh Info
         file.read(reinterpret_cast<char*>(&model.m_MeshInfo), sizeof(Graphics::MeshInfo));

         const auto& vc = model.m_MeshInfo.m_VertexCount;
         const auto& ic = model.m_MeshInfo.m_IndexCount;

         // Read vertices and indices
         file.read(reinterpret_cast<char*>(&vertices), vc * sizeof(Graphics::Vertex));
         file.read(reinterpret_cast<char*>(&indices),  ic * sizeof(uint64_t));

         file.close();
         return model;
    }

    bool ModelLoader::WriteToBinaryFile(const std::string &path, ModelBinaryHeader binaryHeader,
        std::vector<Graphics::Vertex> vertices, std::vector<uint64_t> indices) const
    {
         std::ofstream file(path, std::ios::binary);
         if (!file) {
             Warn("Can't created model binary file: " + path);
             return false;
         }

         // Write magic numbers
         file.write(reinterpret_cast<char*>(binaryHeader.m_Magic), sizeof(uint32_t));

         // Write version
         file.write(reinterpret_cast<char*>(&binaryHeader.m_Version), sizeof(uint32_t));

         // Write Per Mesh data
         size_t count = binaryHeader.m_MeshData.size();
         file.write(reinterpret_cast<char *>(&count), sizeof(count));
         file.write(reinterpret_cast<const char *>(binaryHeader.m_MeshData.data()), count * sizeof(MeshEntry));

         // Write Mesh Info
         file.write(reinterpret_cast<const char *>(&binaryHeader.m_MeshInfo), sizeof(binaryHeader.m_MeshInfo));

         const auto& vc = binaryHeader.m_MeshInfo.m_VertexCount;
         const auto& ic = binaryHeader.m_MeshInfo.m_IndexCount;

         file.write(reinterpret_cast<char*>(vertices.data()), vc * sizeof(Graphics::Vertex));
         file.write(reinterpret_cast<char*>(indices.data()),  ic * sizeof(uint64_t));

         file.close();
         return true;
    }
}
