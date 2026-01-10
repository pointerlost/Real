#ifndef BUFFERS_GLSL
#define BUFFERS_GLSL

struct Transform {
    mat4 modelMatrix;
    mat4 normalMatrix; // unused w (padding)
};
layout(std430, binding = 2) buffer TransformSSBO {
    Transform transforms[];
};
mat4 GetModelMatrix(int idx)  { return transforms[idx].modelMatrix; }
mat3 GetNormalMatrix(int idx) { return mat3(transforms[idx].normalMatrix); }

layout(std140, binding = 3) uniform CameraUBO {
    vec4 position;
    mat4 view;
    mat4 projection;
    mat4 viewProjection;
} uCamera;

vec3 GetViewPos()    { return uCamera.position.xyz;   }
mat4 GetProjView()   { return uCamera.viewProjection; }
mat4 GetView()       { return uCamera.view;           }
mat4 GetProjection() { return uCamera.projection;     }

struct TexturePack {
    vec3 albedo;
    float roughness;
    float metallic;
    float ao;
};

struct Material {
    // Texture Override Colors
    vec4 m_BaseColorFactor;
    vec4 m_ORMFactor; // 0 = normal, 1 = ao, 2 = roughness, 3 = metallic

    // Texture index
    int m_BindlessAlbedoIdx;
    int m_BindlessNormalIdx;
    int m_BindlessORMIdx;
    int m_BindlessHeightIdx;
    int m_BindlessEmissiveIdx;
    int pad1_, pad2_, pad3_;
};
layout(std430, binding = 4) buffer MaterialSSBO {
    Material materials[];
};

// Default values
const vec3  DEFAULT_ALBEDO = vec3(0.8, 0.8, 0.8);
const vec3  DEFAULT_NORMAL = vec3(0.5, 0.5, 1.0); // Flat normal (Z-up)
const float DEFAULT_ROUGHNESS = 0.5;
const float DEFAULT_METALLIC = 0.0;
const float DEFAULT_AO = 1.0;
const float DEFAULT_HEIGHT = 0.0;

// Helper function to check if texture index is valid
bool IsValidTextureIdx(int idx) {
    return idx >= 0;
}

// Material property getters
vec3 GetMaterialBaseColorFactor(int idx) { return materials[idx].m_BaseColorFactor.rgb; }
float GetMaterialAOFactor(int idx) { return materials[idx].m_ORMFactor[0]; }
float GetMaterialRGHFactor(int idx) { return materials[idx].m_ORMFactor[1]; }
float GetMaterialMTLFactor(int idx) { return materials[idx].m_ORMFactor[2]; }
int GetAlbedoTexIdx(int idx) { return materials[idx].m_BindlessAlbedoIdx; }
int GetNormalTexIdx(int idx) { return materials[idx].m_BindlessNormalIdx; }
int GetORMTexIdx(int idx) { return materials[idx].m_BindlessORMIdx; }
int GetHeightTexIdx(int idx) { return materials[idx].m_BindlessHeightIdx; }

layout (std430, binding = 5) buffer TextureBuffer {
    uint64_t bindlessTextures[];
};

// Albedo sampling with fallback
vec3 GetAlbedoSampler2D(int matIdx, vec2 UV) {
    int texIdx = GetAlbedoTexIdx(matIdx);
    if (IsValidTextureIdx(texIdx)) {
        uint64_t handle = bindlessTextures[texIdx];
        return pow(texture(sampler2D(handle), UV).rgb, vec3(2.2));
    }
    // Fallback to material's albedo default
    return DEFAULT_ALBEDO;
}

// Normal sampling with fallback
vec3 GetNormalSampler2D(int matIdx, vec2 UV) {
    int texIdx = GetNormalTexIdx(matIdx);
    if (IsValidTextureIdx(texIdx)) {
        uint64_t handle = bindlessTextures[texIdx];
        return texture(sampler2D(handle), UV).rgb * 2.0 - 1.0;
    }
    // Fallback to flat normal (Z-up)
    return DEFAULT_NORMAL;
}

bool HasNormalMap(int matIdx) {
    return IsValidTextureIdx(GetNormalTexIdx(matIdx));
}

// Roughness sampling with fallback
float GetRoughnessSampler2D(int matIdx, vec2 UV) {
    int texIdx = GetORMTexIdx(matIdx);
    if (IsValidTextureIdx(texIdx)) {
        uint64_t handle = bindlessTextures[texIdx];
        return texture(sampler2D(handle), UV).r;
    }
    // Fallback to material's roughness default
    return DEFAULT_ROUGHNESS;
}

// Metallic sampling with fallback
float GetMetallicSampler2D(int matIdx, vec2 UV) {
    int texIdx = GetORMTexIdx(matIdx);
    if (IsValidTextureIdx(texIdx)) {
        uint64_t handle = bindlessTextures[texIdx];
        return texture(sampler2D(handle), UV).g;
    }
    // Fallback to material's metallic default
    return DEFAULT_METALLIC;
}

// AO sampling with fallback
float GetAOSampler2D(int matIdx, vec2 UV) {
    int texIdx = GetORMTexIdx(matIdx);
    if (IsValidTextureIdx(texIdx)) {
        uint64_t handle = bindlessTextures[texIdx];
        return texture(sampler2D(handle), UV).b;
    }
    // Fallback to material's AO default
    return DEFAULT_AO;
}

// Height sampling with fallback
float GetHeightSampler2D(int matIdx, vec2 UV) {
    int texIdx = GetHeightTexIdx(matIdx);
    if (IsValidTextureIdx(texIdx)) {
        uint64_t handle = bindlessTextures[texIdx];
        return texture(sampler2D(handle), UV).r;
    }
    // Default height (no displacement)
    return DEFAULT_HEIGHT;
}

TexturePack GetTexturePack(int materialIndex, vec2 UV) {
    // Mix with texture's override color(Factors) for GUI
    vec3  albedo    = GetAlbedoSampler2D(materialIndex, UV)    * GetMaterialBaseColorFactor(materialIndex);
    float roughness = GetRoughnessSampler2D(materialIndex, UV) * GetMaterialRGHFactor(materialIndex);
    float metallic  = GetMetallicSampler2D(materialIndex, UV)  * GetMaterialMTLFactor(materialIndex);
    float ao        = GetAOSampler2D(materialIndex, UV)        * GetMaterialAOFactor(materialIndex);
    return TexturePack(albedo, roughness, metallic, ao);
}

struct Light {
    vec4 pos_cutoff; // vec3 position,  w = cutOff
    vec4 dir_outer;  // vec3 direction, w = outerCutOff
    vec4 radiance;   // vec3 radiance,  w = unused
    int type; // point = 0, directional = 1, spot = 2
    // Attenuation parameters
    float constant;
    float linear;
    float quadratic;
};
layout(std430, binding = 6) buffer LightSSBO {
    Light lights[];
};

vec3 GetLightPos(int idx) { return lights[idx].pos_cutoff.xyz;  }
vec3 GetLightDir(int idx) { return normalize(-lights[idx].dir_outer.xyz); }
float GetCutOff(int idx)       { return lights[idx].pos_cutoff.w; }
float GetOuterCutOff(int idx)  { return lights[idx].dir_outer.w;  }
vec3 GetRadiance(int idx)      { return lights[idx].radiance.xyz; }
int GetLightType(int idx)      { return lights[idx].type;         }
float GetLightConstant(int idx)  { return lights[idx].constant;   }
float GetLightLinear(int idx)    { return lights[idx].linear;     }
float GetLightQuadratic(int idx) { return lights[idx].quadratic;  }

// MULTI DRAW INDIRECT BUFFERS
struct DrawElementsIndirectCommand {
    uint count;
    uint instanceCount;
    uint firstIndex;
    uint baseVertex;
    uint baseInstance;
};
layout (std430, binding = 0) buffer IndirectBuffer {
    DrawElementsIndirectCommand indirectCommands[];
};

struct EntityData {
    int transformIndex;
    int materialIndex;
    int indexCount;
    int indexOffset;
};
layout (std430, binding = 1) buffer EntityMetaData {
    EntityData entityData[];
};

// Global Data
layout(std140, binding = 7) uniform GlobalDataUBO {
    vec4 GlobalAmbient; // last index padding
    int lightCount[4]; // 0 = lightCount, other indices padding
} uGlobalData;

vec3 GetGlobalAmbient() { return uGlobalData.GlobalAmbient.xyz; }
int GetLightCount() { return uGlobalData.lightCount[0]; }

#endif