//
// Created by pointerlost on 10/30/25.
//
#pragma once
#include <string>
#include "compressonator/include/cmp_compressonatorlib/compressonator.h"
#include "glad/include/glad/glad.h"
#include <assimp/material.h>
#include "Graphics/Texture.h"
#include <PxActor.h>

namespace Real {
    struct Transform;

    struct TextureData;
    enum class TextureFilterMode;
    enum class TextureResolution;
    enum class TextureType;
    enum class TextureWrapMode;

    namespace math {
        struct Quat;
        struct Vec3;
    }

    namespace physics {
        enum class ColliderShape;
    }
}

namespace Real { struct FileInfo; }

namespace Real::util {
    /* ***************************************** TEXTURE STATE  ************************************** */
    String ImageFormatState_EnumToString(ImageFormatState state);
    String TextureType_EnumToString(TextureType type);
    String GetDefaultTextureName(TextureType type, int width);
    String FormatToString(int format);
    String InternalFormatToString(int format);
    ImageFormatState ImageFormatState_StringToEnum(String state);
    CMP_FORMAT GetCMPDestinationFormat(int channelCount);
    TextureType TextureType_StringToEnum(const String& type);
    uint TexFormat_uncompressed_GetBitPerTexel(TextureType type);
    uint TexFormat_uncompressed_GetBytePerTexel(TextureType type);
    GLenum TextureWrapModeToGLEnum(TextureWrapMode mode);
    GLenum TextureFilterModeToGLEnum(TextureFilterMode filter);
    TextureType AssimpTextureTypeToRealType(aiTextureType type);
    int TextureTypeToChannelCount(TextureType type);
    int GetGLFormat(int channelCount, bool srgb = false);
    int GetCompressedInternalFormat(int channelCount);
    int GetGLInternalFormat(int channelCount, bool srgb = false);
    TextureData ExtractChannel(const TextureData& data, int channelIndex);
    TextureData ExtractChannel(void* data, int width, int height, int channels, int channelIndex);
    TextureData ExtractChannels(const TextureData& data, const Vector<int>& wantedChannels);
    TextureData ExtractChannels(void* data, int width, int height, int channels, const Vector<int>& wantedChannels);

    /* ***************************************** APIs DEBUG STATE  ************************************** */
    void DebugGLError();
    String DebugCMPStatus(CMP_ERROR error);

    /* ***************************************** ENGINE STATE  ************************************** */
    bool TryParseUUID(const String& strUUID, UUID& uuid);

    /* ***************************************** MIXED STATE  ************************************** */
    bool IsSubString(const String& subStr, const String& string);

    /* ***************************************** PHYSICS STATE  ************************************** */
    physx::PxShape* CreatePhysXShapeFromReal(physx::PxPhysics& px, const physx::PxMaterial* mat, physics::ColliderShape shape);
}
