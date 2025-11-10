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

// TODO: I won't use it bindless textures for now! | #extension GL_ARB_bindless_texture : require
struct Material {
    // Texture Override Colors
    vec4 m_BaseColor;
    vec4 m_NormalRMA; // 0 = normal, 1 = roughness, 2 = metallic, 3 = ambient occlusion

    // Texture index
    ivec2 albedoMapLookupData;
    ivec2 normalMapLookupData;
    ivec2 rmaMapLookupData;
    ivec2 heightMapLookupData;
};
layout(std430, binding = 4) buffer MaterialSSBO {
    Material materials[];
};
vec4 GetMatBaseColor(int idx) { return materials[idx].m_BaseColor; }
float GetMatNormalColor(int idx) { return materials[idx].m_NormalRMA[0]; }
float GetMatRoughnessColor(int idx) { return materials[idx].m_NormalRMA[1]; }
float GetMatMetallicColor(int idx) { return materials[idx].m_NormalRMA[2]; }
float GetMatAOColor(int idx) { return materials[idx].m_NormalRMA[3]; }
ivec2 GetAlbedoLookupData(int idx) { return materials[idx].albedoMapLookupData; }
ivec2 GetNormalMapLookupData(int idx) { return materials[idx].normalMapLookupData; }
ivec2 GetRMAMapLookupData(int idx) { return materials[idx].rmaMapLookupData; }
ivec2 GetHeightMapLookupData(int idx) { return materials[idx].heightMapLookupData; }


// TODO: improvable from the perspective of memory padding
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
layout(std430, binding = 5) buffer LightSSBO {
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

// Texture arrays | Albedo x 5 | Normal x 5 | RMA x 5 | Height x 5
// We are using 5 different resolution for per type (256,512,1024,2048,4096)
uniform sampler2DArray textureMapArrays[20]; // Per-type, Per-resolutions

// Global Data
layout(std140, binding = 6) uniform GlobalDataUBO {
    vec4 GlobalAmbient; // last index padding
    int lightCount[4]; // 0 = lightCount, other indices padding
} uGlobalData;

vec3 GetGlobalAmbient() { return uGlobalData.GlobalAmbient.xyz; }
int GetLightCount() { return uGlobalData.lightCount[0]; }

#endif