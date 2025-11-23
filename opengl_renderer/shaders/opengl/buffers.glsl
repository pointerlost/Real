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


struct Material {
    // Texture Override Colors
    vec4 m_BaseColor;
    vec4 m_NormalRMA; // 0 = normal, 1 = roughness, 2 = metallic, 3 = ambient occlusion

    // Texture index
    int m_BindlessAlbedoIdx;
    int m_BindlessNormalIdx;
    int m_BindlessRMAIdx;
    int m_BindlessHeightIdx;
};
layout(std430, binding = 4) buffer MaterialSSBO {
    Material materials[];
};
vec4 GetMatBaseColor(int idx) { return materials[idx].m_BaseColor; }
float GetMatNormalColor(int idx) { return materials[idx].m_NormalRMA[0]; }
float GetMatRoughnessColor(int idx) { return materials[idx].m_NormalRMA[1]; }
float GetMatMetallicColor(int idx) { return materials[idx].m_NormalRMA[2]; }
float GetMatAOColor(int idx) { return materials[idx].m_NormalRMA[3]; }
int GetAlbedoTexIdx(int idx) { return materials[idx].m_BindlessAlbedoIdx; }
int GetNormalTexIdx(int idx) { return materials[idx].m_BindlessNormalIdx; }
int GetRMATexIdx(int idx) { return materials[idx].m_BindlessRMAIdx; }
int GetHeightTexIdx(int idx) { return materials[idx].m_BindlessHeightIdx; }

layout (std430, binding = 5) buffer TextureBuffer {
    uint64_t bindlessTextures[];
};

vec3 GetAlbedoSampler2D(int matIdx, vec2 UV) {
    uint64_t handle = bindlessTextures[GetAlbedoTexIdx(matIdx)];
    return pow(texture(sampler2D(handle), UV).rgb, vec3(2.2));
}

vec3 GetNormalSampler2D(int matIdx, vec2 UV) {
    uint64_t handle = bindlessTextures[GetNormalTexIdx(matIdx)];
    return texture(sampler2D(handle), UV).rgb;
}

float GetRoughnessSampler2D(int matIdx, vec2 UV) {
    uint64_t handle = bindlessTextures[GetRMATexIdx(matIdx)];
    return texture(sampler2D(handle), UV).r;
}

float GetMetallicSampler2D(int matIdx, vec2 UV) {
    uint64_t handle = bindlessTextures[GetRMATexIdx(matIdx)];
    return texture(sampler2D(handle), UV).g;
}

float GetAOSampler2D(int matIdx, vec2 UV) {
    uint64_t handle = bindlessTextures[GetRMATexIdx(matIdx)];
    return texture(sampler2D(handle), UV).b;
}

float GetHeightSampler2D(int matIdx, vec2 UV) {
    uint64_t handle = bindlessTextures[GetHeightTexIdx(matIdx)];
    return texture(sampler2D(handle), UV).r;
}

// TODO: Improvable memory padding
struct Light {
    vec4 pos_cutoff; // vec3 position,  w = cutOff
    vec4 dir_outer;  // vec3 direction, w = outerCutOff
    vec4 diffuse;
    vec4 specular;
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
float GetCutOff(int idx)      { return lights[idx].pos_cutoff.w; }
float GetOuterCutOff(int idx) { return lights[idx].dir_outer.w;  }
vec3 GetLightDiffuse(int idx)  { return lights[idx].diffuse.xyz;  }
vec3 GetLightSpecular(int idx) { return lights[idx].specular.xyz; }
int GetLightType(int idx)      { return lights[idx].type; }
float GetLightConstant(int idx)  { return lights[idx].constant;  }
float GetLightLinear(int idx)    { return lights[idx].linear;    }
float GetLightQuadratic(int idx) { return lights[idx].quadratic; }

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