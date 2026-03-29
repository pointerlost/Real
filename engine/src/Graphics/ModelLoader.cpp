//
// Created by pointerlost on 12/4/25.
//
#include <Graphics/ModelLoader.h>
#include <fstream>
#include <ranges>
#include <utility>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "Common/Macros.h"
#include "Assets/AssetImporter.h"
#include "Assets/AssetManager.h"
#include "Core/CMakeConfig.h"
#include "Assets/FileManager.h"
#include "Tools/Image/TextureUtils.h"
#include "Core/Logger.h"
#include "Core/Services.h"
#include "Graphics/Material.h"
#include "Assets/MeshManager.h"
#include "Assets/MaterialManager.h"
#include "Assets/TextureManager.h"
#include "Graphics/Model.h"
#include "Platform/OpenGL/OpenGLTexture.h"
#include "Serialization/Binary.h"
#include "Tools/Image/ImageTools.h"

namespace Real::graphics {

    void ModelLoader::LoadAll(const String &rootDir) {
        namespace std_fs = std::filesystem;
        auto& am = Services::GetAssetManager();

        auto IsModelFile = [](const std_fs::path& p) {
            if (!p.has_extension()) return false;
            const auto ext = p.extension().string();
            return ext == ".fbx" || ext == ".gltf" || ext == ".glb";
        };

        if (!std_fs::exists(rootDir)) {
            Warn("[ModelLoader::LoadAll] There is no rootDir for model loading: " + rootDir);
            return;
        }

        Vector<std_fs::path> modelFolders;
        static const std::unordered_set<String> extensions = {
            ".png",
            ".tga",
            ".jpg",
            ".tif",
            ".bmp",
            ".webp"
        };

        for (const auto& entry : std_fs::directory_iterator(rootDir)) {
            if (entry.is_directory() && !am.IsModelExist(entry.path().filename().string())) {
                modelFolders.push_back(entry.path());
            }
        }

        for (const auto& folder : modelFolders) {
            // Clear and rebuild texture index for this model folder
            m_TextureIndex.clear();
            Vector<String> modelsPath;
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

    Ref<Model> ModelLoader::Load(const String &filePath, const String& name, ImageFormatState state)
    {
        if (!fs::File::Exists(filePath)) {
            Warn("Model file not found: " + filePath);
            return nullptr;
        }
        // Reset state
        m_CurrentModel = CreateRef<Model>();
        m_CurrentModel->m_Name = name;
        m_CurrImageFormatState = state;
        m_CurrentModel->m_FileInfo = fs::FileInfoFromPath(filePath);

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

        const auto& binary_path = String(ASSETS_RUNTIME_DIR) + "models/" + m_CurrentModel->m_Name + ".model";

        // Create model binary file
        assets::ModelBinaryHeader binary_file{};
        binary_file.magic     = assets::REAL_MAGIC; // Real magic number
        binary_file.version   = 1;
        binary_file.meshCount = m_CurrentModel->m_MeshUUIDs.size();
        binary_file.uuid      = m_CurrentModel->m_UUID;

        serialization::binary::WriteModel(
            binary_path,
            binary_file,
            m_CurrentModel->m_MeshUUIDs,
            m_CurrentModel->m_MaterialAssetUUIDs
        );
        Services::GetAssetManager().RegisterModel(m_CurrentModel);
        Services::GetAssetImporter().SaveModelToAssetDB(m_CurrentModel);

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
        auto& mm = Services::GetMeshManager();
        // Create containers for vertex data
        Vector<Vertex> vertices;
        Vector<u32> indices;

        auto materialUUID = UUID(0); // TODO: i need to create default material fallback
        if (mesh->mMaterialIndex >= 0) {
            const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            Ref<Material> real_material;

            real_material = ProcessMaterial(material, static_cast<int>(mesh->mMaterialIndex));
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
            vertex.position = { p.x, p.y, p.z };

            // Normal
            if (mesh->HasNormals()) {
                aiVector3D n = normalMat * mesh->mNormals[i];
                vertex.normal = { n.x, n.y, n.z };
            } else {
                vertex.normal = math::Vec3(0.0, 1.0, 0.0);
            }

            // UV
            if (mesh->HasTextureCoords(0)) {
                vertex.UV.x = mesh->mTextureCoords[0][i].x;
                vertex.UV.y = mesh->mTextureCoords[0][i].y;
            } else {
                vertex.UV = math::Vec2(0.0, 0.0);
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
        const auto vertexOffset = mm.GetVerticesCount();
        const auto indexOffset  = mm.GetIndicesCount();

        const UUID meshUUID = mm.CreateSingleMesh(vertices, indices, UUID{}).uuid;
        m_CurrentModel->m_MeshUUIDs.push_back(meshUUID);
        m_CurrentModel->m_MaterialAssetUUIDs.push_back(materialUUID);

        assets::MeshBinaryHeader header;
        header.magic        = assets::REAL_MAGIC;
        header.version      = 1;
        header.uuid         = meshUUID;
        header.materialUUID = materialUUID;
        header.vertexCount  = vertices.size();
        header.indexCount   = indices.size();
        header.vertexOffset = vertexOffset;
        header.indexOffset  = indexOffset;

        const auto meshNameAsUUID = std::to_string(meshUUID);
        const auto& mBinaryPath = String(ASSETS_RUNTIME_DIR) + "meshes/" + meshNameAsUUID + ".mesh";
        serialization::binary::WriteMesh(mBinaryPath, header, vertices, indices);
        Services::GetAssetImporter().SaveMeshToAssetDB(header, meshNameAsUUID);
    }

    Ref<Material> ModelLoader::ProcessMaterial(const aiMaterial *mat, int materialIndex) {
        auto& tm = Services::GetTextureManager();
        auto& mm = Services::GetMaterialManager();

        std::unordered_set<TextureType> processedTypes;
        std::unordered_map<String, std::array<Ref<platform::opengl::OpenGLTexture>, 3>> ormPack;

        aiString matName;
        mat->Get(AI_MATKEY_NAME, matName);
        const auto baseName = matName.length > 0
            ? matName.C_Str()
            : "Material_" + std::to_string(materialIndex);

        const auto& material = mm.CreateBase(m_CurrentModel->m_Name + "_" + baseName);

        String destPath = String(ASSETS_DIR) + "textures/";
        destPath += (m_CurrImageFormatState == ImageFormatState::UNCOMPRESSED)
            ? "uncompressed/" : "compress_me/";

        static const std::unordered_map<TextureType, std::string_view> suffix {
            { TextureType::ALBEDO,            "_ALB"      },
            { TextureType::NORMAL,            "_NRM"      },
            { TextureType::ORM,               "_ORM"      },
            { TextureType::AMBIENT_OCCLUSION, "_AO"       },
            { TextureType::ROUGHNESS,         "_RGH"      },
            { TextureType::METALLIC,          "_MTL"      },
            { TextureType::HEIGHT,            "_HEIGHT"   },
            { TextureType::EMISSIVE,          "_EMISSIVE" }
        };

        // Create texture from a resolved file path - new API
        auto CreateTexture = [&](const String& srcPath, const String& destFilePath, TextureType type)
            -> Ref<platform::opengl::OpenGLTexture>
        {
            auto tex = CreateRef<platform::opengl::OpenGLTexture>();
            tex->SetType(type);
            tex->LoadFromFile(srcPath);

            if (!tex->IsReadyForUpload()) {
                Warn("[ProcessMaterial] Texture not ready: " + srcPath);
                return nullptr;
            }

            // Save copy to our asset folder under the correct name
            tex->SetFileInfo(fs::FileInfoFromPath(destFilePath));
            tools::SaveTextureAsFile(tex.get());

            // Compress - updates CPU mip data and state, no GPU touch
            if (!tools::CompressTexture(tex.get()))
                Warn("[ProcessMaterial] Compression failed: " + destFilePath);

            Services::GetAssetImporter().SaveTextureToAssetDB(tex.get());
            tm.Register(tex->GetUUID(), tex);
            return tex;
        };

        // Route texture into material or ORM pack
        auto AssignTexture = [&](const Ref<platform::opengl::OpenGLTexture>& tex, TextureType type) {
            if (!tex) return;
            switch (type) {
                case TextureType::AMBIENT_OCCLUSION:
                    ormPack[material->m_Name][0] = tex; return;
                case TextureType::ROUGHNESS:
                    ormPack[material->m_Name][1] = tex; return;
                case TextureType::METALLIC:
                    ormPack[material->m_Name][2] = tex; return;
                default: break;
            }
            AddTextureToMaterial(tex, material);
        };

        auto LoadTexture = [&](aiTextureType aiType) {
            if (mat->GetTextureCount(aiType) == 0) return;

            const auto realType = util::texture::AssimpTextureTypeToRealType(aiType);
            if (processedTypes.contains(realType)) return;

            aiString texPath;
            if (mat->GetTexture(aiType, 0, &texPath) != AI_SUCCESS) return;

            String pathStr = fs::NormalizePath(texPath.C_Str());
            const std::filesystem::path p(pathStr);

            // Resolve actual file path from texture index
            String resolvedPath;
            String ext;
            if (m_TextureIndex.contains(p.stem().string())) {
                const auto best = ChooseBest(m_TextureIndex[p.stem().string()]);
                resolvedPath = best.string();
                ext          = best.extension().string();
            } else {
                Warn("[ProcessMaterial] Missing texture: " + p.string()
                    + " for material: " + material->m_Name);
                return;
            }

            if (resolvedPath.empty()) return;

            // Cache hit - texture already processed this session
            auto checkCache = [&](const String& key) -> bool {
                if (!m_CacheProcessedTextures.contains(key)) return false;
                const UUID cachedUUID = m_CacheProcessedTextures[key];
                AssignTexture(tm.GetTexture(cachedUUID, realType), realType);
                return true;
            };
            if (checkCache(resolvedPath) || checkCache(pathStr)) return;

            // ALBEDO_ROUGHNESS packed - split channels
            if (realType == TextureType::ALBEDO_ROUGHNESS) {
                auto src = CreateRef<platform::opengl::OpenGLTexture>();
                src->SetType(TextureType::ALBEDO);
                src->LoadFromFile(resolvedPath);

                if (!src->IsReadyForUpload()) return;

                const auto& raw = src->GetMipLevel(0);

                if (raw.channelCount > 3) {
                    // Split RGB -> albedo, A -> roughness
                    auto albData = util::texture::ExtractChannels(raw, {0, 1, 2});
                    auto rghData = util::texture::ExtractChannel(raw, 3);

                    auto albTex = CreateRef<platform::opengl::OpenGLTexture>();
                    albTex->SetType(TextureType::ALBEDO);
                    albTex->SetOrigin(platform::opengl::OpenGLTexture::TextureOrigin::Packed);
                    albTex->SetImageFormatState(ImageFormatState::UNCOMPRESSED);
                    albTex->SetDebugName(material->m_Name + "_ALB");
                    albTex->CreateFromData(albData, TextureType::ALBEDO);
                    albTex->SetFileInfo(fs::FileInfoFromPath(destPath + material->m_Name + "_ALB" + ext));
                    tools::SaveTextureAsFile(albTex.get());
                    tools::CompressTexture(albTex.get());
                    tm.Register(albTex->GetUUID(), albTex);
                    AddTextureToMaterial(albTex, material);
                    m_CacheProcessedTextures[resolvedPath] = albTex->GetUUID();

                    auto rghTex = CreateRef<platform::opengl::OpenGLTexture>();
                    rghTex->SetType(TextureType::ROUGHNESS);
                    rghTex->SetOrigin(platform::opengl::OpenGLTexture::TextureOrigin::Packed);
                    rghTex->SetImageFormatState(ImageFormatState::UNCOMPRESSED);
                    rghTex->SetDebugName(material->m_Name + "_RGH");
                    rghTex->CreateFromData(rghData, TextureType::ROUGHNESS);
                    ormPack[material->m_Name][1] = rghTex;
                } else {
                    const auto tex = CreateTexture(resolvedPath,
                        destPath + material->m_Name + "_ALB" + ext,
                        TextureType::ALBEDO);
                    AddTextureToMaterial(tex, material);
                    if (tex) m_CacheProcessedTextures[resolvedPath] = tex->GetUUID();
                }

                processedTypes.insert(realType);
                return;
            }

            // Standard texture types
            if (const auto it = suffix.find(realType); it != suffix.end()) {
                const auto outPath = destPath + material->m_Name + String(it->second) + ext;
                const auto tex     = CreateTexture(resolvedPath, outPath, realType);
                AssignTexture(tex, realType);
                if (tex) {
                    m_CacheProcessedTextures[resolvedPath] = tex->GetUUID();
                    m_CacheProcessedTextures[pathStr]      = tex->GetUUID();
                }
            }

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

        // Pack ORM after all channels collected
        for (const auto& [matName, pack] : ormPack) {
            const auto ao  = pack[0] ? pack[0] : tm.GetOrCreateDefault(TextureType::AMBIENT_OCCLUSION);
            const auto rgh = pack[1] ? pack[1] : tm.GetOrCreateDefault(TextureType::ROUGHNESS);
            const auto mtl = pack[2] ? pack[2] : tm.GetOrCreateDefault(TextureType::METALLIC);

            // PackORM packs + compresses internally
            const auto orm = tools::PackORM(ao, rgh, mtl, matName);
            if (!orm || orm->IsDefault()) continue;

            Services::GetAssetImporter().SaveTextureToAssetDB(orm.get());
            tm.Register(orm->GetUUID(), orm);
            material->m_ORM = orm->GetUUID();
        }

        return material;
    }

    void ModelLoader::AddTextureToMaterial(
        const Ref<platform::opengl::OpenGLTexture> &tex,
        const Ref<Material>& material)
    {
        if (!tex) return;
        switch (tex->GetType()) {
            case TextureType::ALBEDO:   material->m_Albedo   = tex->GetUUID(); break;
            case TextureType::NORMAL:   material->m_Normal   = tex->GetUUID(); break;
            case TextureType::HEIGHT:   material->m_Height   = tex->GetUUID(); break;
            case TextureType::EMISSIVE: material->m_Emissive = tex->GetUUID(); break;
            default: break;
        }
    }

    std::filesystem::path ModelLoader::ChooseBest(const Vector<std::filesystem::path> &paths) {
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
