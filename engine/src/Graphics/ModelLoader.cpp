//
// Created by pointerlost on 12/4/25.
//
#include <Graphics/ModelLoader.h>
#include <fstream>
#include <utility>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "Common/Macros.h"
#include "Core/AssetImporter.h"
#include "Core/AssetManager.h"
#include "Core/CMakeConfig.h"
#include "Core/file_manager.h"
#include "Core/Logger.h"
#include "Core/Services.h"
#include "Graphics/Material.h"
#include "Graphics/Model.h"
#include "Serialization/Binary.h"
#include "Tools/ImageTools.h"
#include "Util/Util.h"

namespace Real {

    void ModelLoader::LoadAll(const std::string &rootDir) {
        namespace std_fs = std::filesystem;
        const auto& am = Services::GetAssetManager();

        auto IsModelFile = [](const std_fs::path& p) {
            if (!p.has_extension()) return false;
            const auto ext = p.extension().string();
            return ext == ".fbx" || ext == ".gltf" || ext == ".glb";
        };

        if (!std_fs::exists(rootDir)) {
            Warn("[ModelLoader::LoadAll] There is no rootDir for model loading: " + rootDir);
            return;
        }

        std::vector<std_fs::path> modelFolders;
        static const std::unordered_set<std::string> extensions = {
            ".png",
            ".tga",
            ".jpg",
            ".tif",
            ".bmp",
            ".webp"
        };

        for (const auto& entry : std_fs::directory_iterator(rootDir)) {
            if (entry.is_directory() && !am->IsModelExist(entry.path().filename().string())) {
                modelFolders.push_back(entry.path());
            }
        }

        for (const auto& folder : modelFolders) {
            // Clear and rebuild texture index for this model folder
            m_TextureIndex.clear();
            std::vector<std::string> modelsPath;
            const auto& modelName = folder.filename().string();

            for (const auto& entry : std_fs::recursive_directory_iterator(folder)) {

                if (entry.is_regular_file() && IsModelFile(entry.path())) {
                    m_CurrentDirectory = entry.path().parent_path().string();
                    modelsPath.push_back(entry.path().string());
                }
                else if (entry.is_regular_file()) {
                    if (extensions.contains(entry.path().extension())) {
                        m_TextureIndex[entry.path().stem().string()].push_back(entry.path());
                    }
                }
            }

            for (const auto& modelPath : modelsPath) {
                if (IsModelFile(modelPath)) {
                    // Build texture index from this model's directory
                    std::filesystem::path modelDir = std::filesystem::path(modelPath).parent_path();
                    for (const auto& entry : std_fs::directory_iterator(modelDir)) {
                        if (entry.is_regular_file() && extensions.contains(entry.path().extension())) {
                            m_TextureIndex[entry.path().stem().string()].push_back(entry.path());
                        }
                    }

                    // Check subdirectories
                    for (const auto& entry : std_fs::recursive_directory_iterator(modelDir)) {
                        if (entry.is_regular_file() && extensions.contains(entry.path().extension())) {
                            m_TextureIndex[entry.path().stem().string()].push_back(entry.path());
                        }
                    }

                    m_CurrentDirectory = modelDir.string();

                    if (modelPath.substr(modelPath.size() - 4) == ".fbx")
                        m_IsFBX = true;
                    Load(modelPath, modelName, ImageFormatState::COMPRESS_ME);
                    m_IsFBX = false;
                }
            }
        }
    }

    Ref<Model> ModelLoader::Load(const std::string &filePath, const std::string& name, const ImageFormatState state) {
        if (!fs::File::Exists(filePath)) {
            Warn("Model file not found: " + filePath);
            return nullptr;
        }
        // Reset state
        m_CurrentModel = CreateRef<Model>();
        m_CurrentModel->m_Name = name;
        m_CurrImageFormatState = state;
        m_CurrentModel->m_FileInfo = fs::CreateFileInfoFromPath(filePath);

        // Create assimp importer
        Assimp::Importer importer;

        // Flags for complex models
        unsigned int importFlags =
            aiProcess_Triangulate |
            aiProcess_GenSmoothNormals |
            aiProcess_FlipUVs |
            aiProcess_JoinIdenticalVertices |
            aiProcess_ImproveCacheLocality;   // Better performance

        // TODO: I'll add this flag when I add tangents and bitangents!
        // aiProcess_CalcTangentSpace |      /* For normal mapping */

        if (m_IsFBX) { // Need some optimization because .fbx models are slower than gltf or something
            importFlags &= ~aiProcess_ImproveCacheLocality;
        }

        // Load the scene
        const aiScene* scene = importer.ReadFile(filePath, importFlags);

        // Check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            Warn(std::format("Failed to load model: {} - {}", filePath, importer.GetErrorString()));
            return nullptr;
        }

        // Start processing from root node
        const aiMatrix4x4 identity;
        ProcessNode(scene->mRootNode, scene, identity);

        const auto& binary_path = std::string(ASSETS_RUNTIME_DIR) + "models/" + m_CurrentModel->m_Name + ".model";

        // Create model binary file
        ModelBinaryHeader binary_file{};
        binary_file.m_Magic     = REAL_MAGIC; // Real magic number
        binary_file.m_Version   = 1;
        binary_file.m_MeshCount = m_CurrentModel->m_MeshUUIDs.size();
        binary_file.m_UUID      = m_CurrentModel->m_UUID;

        serialization::binary::WriteModel(
            binary_path,
            binary_file,
            m_CurrentModel->m_MeshUUIDs,
            m_CurrentModel->m_MaterialAssetUUIDs
        );
        Services::GetAssetManager()->SaveModelCPU(m_CurrentModel);
        Services::GetAssetImporter()->SaveModelToAssetDB(m_CurrentModel);

        return m_CurrentModel;
    }

    void ModelLoader::ProcessNode(const aiNode* node, const aiScene* scene, const aiMatrix4x4& parentTransform) {
        const aiMatrix4x4 globalTransform = parentTransform * node->mTransformation;

        // Process all meshes in this node
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            ProcessMesh(mesh, scene, globalTransform);
        }

        // Process all children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            ProcessNode(node->mChildren[i], scene, globalTransform);
        }
    }

    void ModelLoader::ProcessMesh(const aiMesh *mesh, const aiScene *scene, const aiMatrix4x4& transform) {
        const auto& mm = Services::GetMeshManager();
        // Create containers for vertex data
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        auto materialUUID = UUID(0); // TODO: i need to create default material fallback
        if (mesh->mMaterialIndex >= 0) {
            const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            Ref<Material> real_material;

            real_material = ProcessMaterial(material, (int)mesh->mMaterialIndex);
            materialUUID = real_material ? real_material->m_UUID : materialUUID;
        }

        aiMatrix3x3 normalMat(transform);
        normalMat.Inverse().Transpose();

        // Process vertices
        vertices.reserve(mesh->mNumVertices);
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex{};

            // Position
            aiVector3D p = transform * mesh->mVertices[i];
            vertex.m_Position = { p.x, p.y, p.z };

            // Normal
            if (mesh->HasNormals()) {
                aiVector3D n = normalMat * mesh->mNormals[i];
                vertex.m_Normal = { n.x, n.y, n.z };
            } else {
                vertex.m_Normal = glm::vec3(0.0, 1.0, 0.0);
            }

            // UV
            if (mesh->HasTextureCoords(0)) {
                vertex.m_UV.x = mesh->mTextureCoords[0][i].x;
                vertex.m_UV.y = mesh->mTextureCoords[0][i].y;
            } else {
                vertex.m_UV = glm::vec2(0.0, 0.0);
            }

            // Tangent and Bitangent
            if (mesh->HasTangentsAndBitangents()) {
                // TODO: I'll update this condition when I add tangents and bitangents!
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

        // Get offsets before creation current meshes
        const auto vertexOffset = mm->GetVerticesCount();
        const auto indexOffset  = mm->GetIndicesCount();

        const UUID meshUUID = Services::GetMeshManager()->CreateSingleMesh(vertices, indices, UUID{}).m_MeshUUID;
        m_CurrentModel->m_MeshUUIDs.push_back(meshUUID);
        m_CurrentModel->m_MaterialAssetUUIDs.push_back(materialUUID);

        MeshBinaryHeader header;
        header.m_Magic        = REAL_MAGIC;
        header.m_Version      = 1;
        header.m_UUID         = meshUUID;
        header.m_MaterialUUID = materialUUID;
        header.m_VertexCount  = vertices.size();
        header.m_IndexCount   = indices.size();
        header.m_VertexOffset = vertexOffset;
        header.m_IndexOffset  = indexOffset;

        const auto meshNameAsUUID = std::to_string(meshUUID);
        const auto& mBinaryPath = std::string(ASSETS_RUNTIME_DIR) + "meshes/" + meshNameAsUUID + ".mesh";
        serialization::binary::WriteMesh(mBinaryPath, header, vertices, indices);
        Services::GetAssetImporter()->SaveMeshToAssetDB(header, meshNameAsUUID);
    }

    Ref<Material> ModelLoader::ProcessMaterial(const aiMaterial *mat, int materialIndex) {
        const auto& am = Services::GetAssetManager();

        std::unordered_set<TextureType> processedTypes;
        std::unordered_map<std::string, std::array<Ref<OpenGLTexture>, 3>> m_ormPack;

        // Material naming
        aiString matName;
        mat->Get(AI_MATKEY_NAME, matName);
        const auto baseName = matName.length > 0 ? matName.C_Str() : "Material_" + std::to_string(materialIndex);

        const auto& material = am->CreateMaterialBase(m_CurrentModel->m_Name + "_" + baseName);

        std::string destPath = std::string(ASSETS_DIR) + "textures/";
        m_CurrImageFormatState == ImageFormatState::UNCOMPRESSED ? destPath += "uncompressed/" : destPath += "compress_me/";

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

        auto Create = [&](TextureData& td, const std::string& path, const TextureType type) {
            FileInfo fi = fs::CreateFileInfoFromPath(path);
            const auto tex = CreateRef<OpenGLTexture>(td, true, type, ImageFormatState::COMPRESS_ME, fi);
            if (type == TextureType::AMBIENT_OCCLUSION) {
                m_ormPack[material->m_Name][0] = tex;
            }
            if (type == TextureType::ROUGHNESS) {
                m_ormPack[material->m_Name][1] = tex;
            }
            if (type == TextureType::METALLIC) {
                m_ormPack[material->m_Name][2] = tex;
            }
            return tex;
        };


        auto CacheProcessedTextures = [this](const std::string& path1, const std::string& path2, const Ref<OpenGLTexture>& tex) {
            m_CacheProcessedTextures.emplace(path1, tex->GetUUID());
            m_CacheProcessedTextures.emplace(path2, tex->GetUUID());
        };

        auto CreateAndSave = [&](TextureData& td, const std::string& path, const TextureType type) -> Ref<OpenGLTexture>
        {
            const auto tex = Create(td, path, type);
            AddTextureToMaterial(tex, material);
            SaveModelTextureAsFile(tex);
            return tex;
        };

        auto LoadTexture = [&](aiTextureType aiType) {
            if (mat->GetTextureCount(aiType) == 0) return;
            const auto realType = util::AssimpTextureTypeToRealType(aiType);

            if (processedTypes.contains(realType)) return; // Skip if already processed before

            aiString texPath;
            if (mat->GetTexture(aiType, 0, &texPath) != AI_SUCCESS) return;

            std::string pathStr = texPath.C_Str();
            pathStr = fs::NormalizePath(pathStr);
            const std::filesystem::path p(pathStr);

            std::string path;
            std::string ext = p.extension().string();

            if (!pathStr.empty() && pathStr.front() == '*') {
                Warn("There are embedded textures!!" + pathStr);
            }

            if (m_TextureIndex.contains(p.stem().string())) {
                const auto realPath = ChooseBest(m_TextureIndex[p.stem().string()]);
                path = realPath.string();
                ext  = realPath.extension().string();
            } else {
                Warn("Missing texture for material: " + material->m_Name + " | texture path: " + p.string() + " and skipping!");
                return; // Skip if texture does not exist
            }

            // Check if already exists
            if (m_CacheProcessedTextures.contains(path)) {
                AddTextureToMaterial(am->GetTexture(m_CacheProcessedTextures[path], realType), material);
                return;
            }
            if (m_CacheProcessedTextures.contains(pathStr)) {
                AddTextureToMaterial(am->GetTexture(m_CacheProcessedTextures[pathStr], realType), material);
                return;
            }

            if (path.empty()) return;

            TextureData texData = am->LoadTextureFromFile(path, realType);
            if (!texData.m_Data) return;

            if (realType == TextureType::ALBEDO_ROUGHNESS && texData.m_ChannelCount > 3) {
                auto alb = util::ExtractChannels(texData, {0,1,2});
                const auto albTex = CreateAndSave(alb,
                    destPath + material->m_Name + "_ALB" + ext,
                    TextureType::ALBEDO
                );
                AddTextureToMaterial(albTex, material);
                CacheProcessedTextures(path, pathStr, albTex);

                auto rghData = util::ExtractChannel(texData, 3);
                const auto rgh = Create(rghData,
                    destPath + material->m_Name + "_RGH" + ext,
                    TextureType::ROUGHNESS
                );
                return;
            }
            if (realType == TextureType::ALBEDO_ROUGHNESS) {
                const auto tex = CreateAndSave(texData, destPath + material->m_Name + "_ALB" + ext, TextureType::ALBEDO);
                AddTextureToMaterial(tex, material);
                CacheProcessedTextures(path, pathStr, tex);
                return;
            }

            if (const auto it = suffix.find(realType); it != suffix.end()) {
                const auto outPath = destPath + material->m_Name + std::string(it->second) + ext;
                if (realType == TextureType::AMBIENT_OCCLUSION ||
                    realType == TextureType::ROUGHNESS         ||
                    realType == TextureType::METALLIC)
                {
                    Create(texData, outPath, it->first);
                }
                else {
                    const auto tex = CreateAndSave(texData, outPath, it->first);
                    AddTextureToMaterial(tex, material);
                    CacheProcessedTextures(path, pathStr, tex);
                }
            }

            // Cache textures
            processedTypes.insert(realType);
        };

        LoadTexture(aiTextureType_BASE_COLOR);
        LoadTexture(aiTextureType_DIFFUSE);
        LoadTexture(aiTextureType_SPECULAR);
        LoadTexture(aiTextureType_SHININESS);
        LoadTexture(aiTextureType_REFLECTION);
        LoadTexture(aiTextureType_NORMAL_CAMERA);
        LoadTexture(aiTextureType_NORMALS);
        LoadTexture(aiTextureType_METALNESS);
        LoadTexture(aiTextureType_DIFFUSE_ROUGHNESS);
        LoadTexture(aiTextureType_GLTF_METALLIC_ROUGHNESS);
        LoadTexture(aiTextureType_AMBIENT_OCCLUSION);
        LoadTexture(aiTextureType_LIGHTMAP);
        LoadTexture(aiTextureType_EMISSIVE);
        LoadTexture(aiTextureType_HEIGHT);
        LoadTexture(aiTextureType_DISPLACEMENT);
        LoadTexture(aiTextureType_OPACITY);

        // Save ORM(Packed) Textures
        for (const auto& pack : std::views::values(m_ormPack)) {
            const auto ao  = pack[0] ? pack[0] : am->GetOrCreateDefaultTexture(TextureType::AMBIENT_OCCLUSION);
            const auto rgh = pack[1] ? pack[1] : am->GetOrCreateDefaultTexture(TextureType::ROUGHNESS);
            const auto mtl = pack[2] ? pack[2] : am->GetOrCreateDefaultTexture(TextureType::METALLIC);
            const auto orm = tools::PackTexturesToRGBChannels({ao, rgh, mtl}, material->m_Name);
            if (orm && orm->GetImageFormatState() != ImageFormatState::DEFAULT) {
                tools::CompressTextureAndReadFromFile(orm.get());
                material->m_ORM = orm->GetUUID();
            }
        }

        return material;
    }

    void ModelLoader::AddTextureToMaterial(const Ref<OpenGLTexture> &tex, const Ref<Material>& material) {
        switch (tex->GetType()) {
            case TextureType::ALBEDO:   material->m_Albedo   = tex->GetUUID(); break;
            case TextureType::NORMAL:   material->m_Normal   = tex->GetUUID(); break;
            case TextureType::HEIGHT:   material->m_Height   = tex->GetUUID(); break;
            case TextureType::EMISSIVE: material->m_Emissive = tex->GetUUID(); break;
            default: ;
        }
    }

    void ModelLoader::SaveModelTextureAsFile(const Ref<OpenGLTexture>& tex) {
        if (tex->GetImageFormatState() != ImageFormatState::DEFAULT) {
            tools::SaveTextureAsFile(tex.get());
            tools::CompressTextureAndReadFromFile(tex.get());
            // Save after compression to use compressed data
            Services::GetAssetManager()->SaveTextureCPU(tex);
        }
    }

    std::filesystem::path ModelLoader::ChooseBest(const std::vector<std::filesystem::path> &paths) {
        // Prefer by order
        static constexpr std::array<std::string_view, 7> priority = {
            ".png",
            ".tga",
            ".jpg",
            ".jpeg",
            ".tif",
            ".bmp",
            ".webp"
        };

        for (auto& ext : priority) {
            for (auto& p : paths) {
                auto e = p.extension().string();
                std::ranges::transform(e, e.begin(), ::tolower);
                if (e == ext)
                    return p;
            }
        }

        if (paths.empty()) return {};
        return paths.front();
    }

}
