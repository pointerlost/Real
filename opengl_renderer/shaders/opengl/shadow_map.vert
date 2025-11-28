#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aUV;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 UV;
    vec4 FragPosLightSpace;
} vs_out;

layout (std430, binding = 7) buffer SHADOW_MAP_SSBO {
    mat4 projectionView;
    mat4 model;
    mat4 lightSpaceMatrix;
} shadowBuffer;


void main() {
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
    vs_out.UV = aUV;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    gl_Position = shadowBuffer.projectionView * vec4(vs_out.FragPos, 1.0);
}