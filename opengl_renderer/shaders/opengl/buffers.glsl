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

// TODO: #extension GL_ARB_bindless_texture : require
struct Material {
    vec4 baseColor;
    vec4 emissiveMetallic;
    float roughnessTexLayer[4]; // 0 = roughness, 1 = texture layer, other indices padding (16-byte alignment)
};
layout(std430, binding = 4) buffer MaterialSSBO {
    Material materials[];
};

vec4 GetBaseColor(int idx)  { return materials[idx].baseColor; }
vec3 GetEmissive(int idx)   { return materials[idx].emissiveMetallic.xyz; }
float GetMetallic(int idx)  { return materials[idx].emissiveMetallic.w; }
float GetRoughness(int idx) { return materials[idx].roughnessTexLayer[0]; }
float GetTextureLayer(int idx) { return materials[idx].roughnessTexLayer[1]; }
float GetTextureLayer2(int idx) { return materials[idx].roughnessTexLayer[2]; }

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
vec3 GetLightDir(int idx) { return normalize(lights[idx].dir_outer.xyz); }
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

// Texture array
layout (binding = 6) uniform sampler2DArray u_TextureArray;

#endif