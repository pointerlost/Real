#version 460
// Before you include any file, pay attention to the order! (preprocessors work like recursion)
#include "opengl/utils.glsl"
#include "opengl/buffers.glsl"
#include "opengl/lighting_calc.glsl"

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 UV;
    flat int MaterialIndex;
} fs_in;

out vec4 FragColor;

void main() {
    int lightCount = GetLightCount();
    vec3 result = vec3(0.0);

    vec3 normal = normalize(fs_in.Normal);
    vec3 matColor = GetMatBaseColor(fs_in.MaterialIndex).rgb;
    vec3 aoColor = texture(textureMapArrays[GetRMAMapLookupData(fs_in.MaterialIndex).x], vec3(fs_in.UV, GetRMAMapLookupData(fs_in.MaterialIndex).y)).rgb;

    for (int i = 0; i < lightCount; i++) {
        vec3 lightDir = GetLightDir(i);
        vec3 viewDir = normalize(GetViewPos() - fs_in.FragPos);
        vec3 halfwayDir = normalize(lightDir + viewDir);

        float NdotH = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

        // vec3 texDiff = texture(u_TextureArray, vec3(fs_in.UV, texDiffLayer)).rgb;
        // vec3 texSpec = texture(u_TextureArray, vec3(fs_in.UV, texSpecLayer)).rgb;

        // if (texDiffLayer < 0.0) texDiff = matColor;
        // if (texSpecLayer < 0.0) texSpec = vec3(0.0, 0.0, 0.0);

        result += CalculateLighting(i, fs_in.Normal, NdotH, fs_in.FragPos, vec3(1.0), vec3(1.0), GetGlobalAmbient(), matColor);
    }

    // Call SRGB_to_LinearSpace function to get correct result, this function applying Gamma Correction
    FragColor = vec4(Convert_LinearSpace_to_sRGB(result), 1.0) * aoColor;
}
