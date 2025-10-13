#ifndef BUFFERS_GLSL
#define BUFFERS_GLSL

constexpr int MAX_ENTITIES = 16384;
constexpr int MAX_LIGHT = 512;

struct Transform {
    mat4 modelMatrix;
};
layout(std430, binding = 2) buffer TransformSSBO {
    Transform transforms[MAX_ENTITIES];
    // TODO: Add normal matrix!!
};

layout(std140, binding = 3) uniform CameraUBO {
    mat4 view;
    mat4 projection;
    mat4 viewProjection;
};

struct Material {
    vec4 baseColor;
    vec4 emissiveMetallic;
    float roughness[4]; // 0 = roughness, other indices padding (16-byte alignment)
};
layout(std430, binding = 4) buffer MaterialSSBO {
    Material materials[MAX_ENTITIES];
};

vec4 GetBaseColor(int idx) { return materials[idx].baseColor; }
vec3 GetEmissive(int idx)  { return materials[idx].emissiveMetallic.xyz; }
float GetMetallic(int idx) { return materials[idx].emissiveMetallic.w; }
float GetRoughness(int idx) { return materials[idx].roughness[0]; }

struct Light {
    vec4 diffuse;  // w = padding
    vec4 specular; // w = padding
};
layout(std430, binding = 5) buffer LightSSBO {
    Light lights[MAX_LIGHT];
};

vec3 GetDiffuse(int idx)  { return lights[idx].diffuse.xyz;  }
vec3 GetSpecular(int idx) { return lights[idx].specular.xyz; }

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

struct EntityMetadata {
    uint transformIndex;
    uint materialIndex;
    uint indexCount;
    uint indexOffset;
};
layout (std430, binding = 1) buffer EntityData {
    EntityMetadata entityData[];
};

#endif