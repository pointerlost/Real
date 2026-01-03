#version 460 core
#extension GL_ARB_shader_draw_parameters : enable
#extension GL_ARB_gpu_shader_int64 : enable
#extension GL_NV_gpu_shader5 : enable

#include "opengl/buffers.glsl"

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 UV;
    flat int MaterialIndex;
} vs_out;

void main() {
    int entityIdx = gl_DrawID;
    EntityData entityProps = entityData[entityIdx];

    int transformIdx = entityProps.transformIndex;

    vs_out.MaterialIndex = entityProps.materialIndex;
    vs_out.FragPos = vec3(GetModelMatrix(transformIdx) * vec4(aPos, 1.0));
    vs_out.Normal = normalize(GetNormalMatrix(transformIdx) * aNormal);
    vs_out.UV = aUV;

    gl_Position = GetProjView() * GetModelMatrix(transformIdx) * vec4(aPos, 1.0f);
}