#version 460
#include "opengl/buffers.glsl"

// TODO: #extension GL_ARB_bindless_texture : require

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

out vec3 vFragPos;
out vec3 vNormal;
out vec2 vUV;

uniform mat4 View;
uniform mat4 Projection;

flat out int vMaterialIndex;

void main() {
    int entityIdx = gl_BaseInstance;
    EntityData entityProps = entityData[entityIdx];
    vMaterialIndex = entityProps.materialIndex;
    int transformIdx = entityProps.transformIndex;

    vFragPos = vec3(GetModelMatrix(transformIdx) * vec4(aPos, 1.0));
    vNormal = GetNormalMatrix(transformIdx) * aNormal;
    vUV = aUV;
    gl_Position = Projection * View * GetModelMatrix(transformIdx) * vec4(aPos, 1.0f);
}