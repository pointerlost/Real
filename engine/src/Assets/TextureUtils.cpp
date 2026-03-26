//
// Created by pointerlost on 3/23/26.
//
#include "Assets/TextureUtils.h"
#include "Core/Logger.h"
#include "Platform/OpenGL/OpenGLUtils.h"

namespace Real::util::texture {

    int TextureTypeToChannelCount(TextureType type) {
        switch (type) {
            case TextureType::ALBEDO:
            case TextureType::NORMAL:
            case TextureType::ORM:
            case TextureType::EMISSIVE:
            case TextureType::ALBEDO_ROUGHNESS:
            case TextureType::METALLIC_ROUGHNESS:
                return 4;

            case TextureType::AMBIENT_OCCLUSION:
            case TextureType::ROUGHNESS:
            case TextureType::METALLIC:
            case TextureType::HEIGHT:
            case TextureType::ALPHA:
                return 1;

            default: {
                Warn("No channel count for type: " + TextureType_EnumToString(type));
                return 1;
            }
        }
    }

    TextureType TextureType_StringToEnum(const String& type) {
        if (type == "albedo"   || type == "ALB")      return TextureType::ALBEDO;
        if (type == "normal"   || type == "NRM")      return TextureType::NORMAL;
        if (type == "orm"      || type == "ORM")      return TextureType::ORM;
        if (type == "height"   || type == "HEIGHT")   return TextureType::HEIGHT;
        if (type == "emissive" || type == "EMISSIVE") return TextureType::EMISSIVE;
        if (type == "ao"       || type == "AO")       return TextureType::AMBIENT_OCCLUSION;
        if (type == "roughness"|| type == "RGH")      return TextureType::ROUGHNESS;
        if (type == "metallic" || type == "MTL")      return TextureType::METALLIC;
        if (type == "albedo_roughness"   || type == "ALB_RGH")  return TextureType::ALBEDO_ROUGHNESS;
        if (type == "alpha"              || type == "ALPHA")    return TextureType::ALPHA;
        if (type == "metallic_roughness" || type == "MTL_RGH")  return TextureType::METALLIC_ROUGHNESS;

        Info("[TextureType_StringToEnum] Returning UNDEFINED for: " + type);
        return TextureType::UNDEFINED;
    }

    String TextureType_EnumToString(TextureType type) {
        switch (type) {
            case TextureType::ALBEDO:             return "ALB";
            case TextureType::NORMAL:             return "NRM";
            case TextureType::ORM:                return "ORM";
            case TextureType::HEIGHT:             return "HEIGHT";
            case TextureType::EMISSIVE:           return "EMISSIVE";
            case TextureType::AMBIENT_OCCLUSION:  return "AO";
            case TextureType::ROUGHNESS:          return "RGH";
            case TextureType::METALLIC:           return "MTL";
            case TextureType::ALBEDO_ROUGHNESS:   return "ALB_RGH";
            case TextureType::ALPHA:              return "ALPHA";
            case TextureType::METALLIC_ROUGHNESS: return "MTL_RGH";

            default: {
                Warn("[TextureType_EnumToString] Returning UNDEFINED");
                return "UNDEFINED";
            }
        }
    }

    String GetDefaultTextureName(TextureType type, int width) {
        return "default_" + TextureType_EnumToString(type) + "_" + std::to_string(width);
    }

    uint GetBitPerTexel(TextureType type) {
        switch (type) {
            case TextureType::ALBEDO:
            case TextureType::ALBEDO_ROUGHNESS:
            case TextureType::NORMAL:
            case TextureType::ORM:
                return 32;

            case TextureType::ROUGHNESS:
            case TextureType::METALLIC:
            case TextureType::AMBIENT_OCCLUSION:
            case TextureType::HEIGHT:
            case TextureType::EMISSIVE:
                return 8;

            default: return 1;
        }
    }

    uint GetBytePerTexel(TextureType type) {
        return GetBitPerTexel(type) / 8;
    }

    TextureType AssimpTextureTypeToRealType(aiTextureType type) {
        switch (type) {
            case aiTextureType_DIFFUSE:
            case aiTextureType_BASE_COLOR:              return TextureType::ALBEDO;

            case aiTextureType_NORMAL_CAMERA:
            case aiTextureType_NORMALS:                 return TextureType::NORMAL;

            case aiTextureType_DISPLACEMENT:
            case aiTextureType_HEIGHT:                  return TextureType::HEIGHT;

            case aiTextureType_LIGHTMAP:
            case aiTextureType_AMBIENT_OCCLUSION:       return TextureType::AMBIENT_OCCLUSION;

            case aiTextureType_SPECULAR:
            case aiTextureType_SHININESS:               return TextureType::ROUGHNESS;

            case aiTextureType_AMBIENT:
            case aiTextureType_OPACITY:                 return TextureType::ALPHA;

            case aiTextureType_METALNESS:               return TextureType::METALLIC;
            case aiTextureType_EMISSIVE:                return TextureType::EMISSIVE;
            case aiTextureType_DIFFUSE_ROUGHNESS:       return TextureType::ALBEDO_ROUGHNESS;
            case aiTextureType_GLTF_METALLIC_ROUGHNESS: return TextureType::METALLIC_ROUGHNESS;

            default: return TextureType::UNDEFINED;
        }
    }

    graphics::TextureData ExtractChannel(const graphics::TextureData& data, int channelIndex) {
        if (channelIndex < 0 || channelIndex >= data.channelCount) {
            Warn("[ExtractChannel] Invalid channel index!");
            return {};
        }

        graphics::TextureData d;
        d.width          = data.width;
        d.height         = data.height;
        d.channelCount   = 1;
        d.dataSize       = data.width * data.height * 1;
        d.data           = new u8[d.dataSize];
        d.format         = opengl::GetGLFormat(d.channelCount);
        d.internalFormat = opengl::GetGLInternalFormat(d.channelCount);

        const auto* src = static_cast<const u8*>(data.data);
        auto*       dst = static_cast<u8*>(d.data);

        for (int i = 0; i < data.width * data.height; i++)
            dst[i] = src[i * data.channelCount + channelIndex];

        return d;
    }

    graphics::TextureData ExtractChannel(void* data, int width, int height, int channels, int channelIndex) {
        graphics::TextureData wrapper;
        wrapper.data           = data;
        wrapper.width          = width;
        wrapper.height         = height;
        wrapper.channelCount   = channels;
        wrapper.format         = opengl::GetGLFormat(channels);
        wrapper.internalFormat = opengl::GetGLInternalFormat(channels);

        return ExtractChannel(wrapper, channelIndex);
    }

    graphics::TextureData ExtractChannels(const graphics::TextureData& data, const Vector<int>& wantedChannels) {
        const int outC = static_cast<int>(wantedChannels.size());

        graphics::TextureData d;
        d.width          = data.width;
        d.height         = data.height;
        d.channelCount   = outC;
        d.dataSize       = data.width * data.height * outC;
        d.data           = new u8[d.dataSize];
        d.format         = opengl::GetGLFormat(d.channelCount);
        d.internalFormat = opengl::GetGLInternalFormat(d.channelCount);

        const auto* src = static_cast<const u8*>(data.data);
        auto*       dst = static_cast<u8*>(d.data);

        for (int i = 0; i < data.width * data.height; i++)
            for (int j = 0; j < outC; j++)
                dst[i * outC + j] = src[i * data.channelCount + wantedChannels[j]];

        return d;
    }

    graphics::TextureData ExtractChannels(void* data, int width, int height, int channels, const Vector<int>& wanted) {
        graphics::TextureData wrapper;
        wrapper.data         = data;
        wrapper.width        = width;
        wrapper.height       = height;
        wrapper.channelCount = channels;

        return ExtractChannels(wrapper, wanted);
    }

}
