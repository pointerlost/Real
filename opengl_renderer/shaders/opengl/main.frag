#version 460
// Before you include any file, pay attention to the order! (preprocessors work like recursion)
#include "opengl/buffers.glsl"
#include "opengl/lighting_calc.glsl"

in vec3 vFragPos;
in vec3 vNormal;
in vec2 vUV;
flat in int vMaterialIndex;

uniform vec3 g_GlobalAmbient;
uniform int uLightCount;

out vec4 FragColor;

void main() {
    vec3 result = vec3(0.0);
    vec3 normal = vNormal;
    float texDiffLayer = GetTextureLayer(vMaterialIndex);
    float texSpecLayer = GetTextureLayer2(vMaterialIndex);
    vec3 matColor = GetBaseColor(vMaterialIndex).rgb;

    for (int i = 0; i < uLightCount; i++) {
        vec3 lightDir = GetLightDir(i);
        vec3 reflectDir = reflect(-lightDir, normal);
        vec3 viewDir = normalize(GetViewPos() - vFragPos);

        float NdotL = max(dot(normal, lightDir), 0.0);
        float RdotV = max(dot(reflectDir, viewDir), 0.0);

        vec3 texDiff = texture(u_TextureArray, vec3(vUV, texDiffLayer)).rgb;
        vec3 texSpec = texture(u_TextureArray, vec3(vUV, texSpecLayer)).rgb;

        if (texDiffLayer < 0.0) texDiff = vec3(1.0, 1.0, 1.0);
        if (texSpecLayer < 0.0) texSpec = vec3(0.0, 0.0, 0.0);

        result += CalculateLighting(i, NdotL, RdotV, vFragPos, texDiff, texSpec, g_GlobalAmbient, matColor);
    }

    FragColor = vec4(result, 1.0);
}
