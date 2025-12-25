#version 460 core
#extension GL_ARB_bindless_texture : enable
#extension GL_ARB_gpu_shader_int64 : enable

struct PerVertexData {
    vec3 fragPos;
    int  materialIndex;
    vec3 normal;
    vec2 UV;
};

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 UV;
    flat int MaterialIndex;
} fs_in;

// Before you include any file, pay attention to the order! (preprocessors work like recursion)
#include "opengl/utils.glsl"
#include "opengl/buffers.glsl"
#include "opengl/lighting_calc.glsl"

out vec4 FragColor;

void main() {
    PerVertexData pvd = PerVertexData(fs_in.FragPos, fs_in.MaterialIndex, fs_in.Normal, fs_in.UV);

    vec3 normal   = normalize(fs_in.Normal);
    vec3 matColor = GetMatBaseColor(fs_in.MaterialIndex).rgb;

    TexturePack tp = GetTexturePack(fs_in.MaterialIndex, fs_in.UV);
    tp.albedo *= matColor; // Mix with texture's override color for GUI

    // Get normal from normal map if available, otherwise use vertex normal
    vec3 sampledNormal = GetNormalSampler2D(fs_in.MaterialIndex, fs_in.UV);
    vec3 N = (length(sampledNormal) > 0.1) ?
             GetNormalFromMap(sampledNormal, fs_in.FragPos, normal, fs_in.UV) :
             normal;

    vec3 V  = normalize(GetViewPos() - fs_in.FragPos);
    vec3 F0 = mix(vec3(0.04), tp.albedo, tp.metallic);

    // Direct lighting
    vec3 Lo = vec3(0.0);
    int lightCount = GetLightCount();
    for (int i = 0; i < lightCount; i++) {
        Lo += CalculatePBR(lights[i], pvd, N, F0, tp);
    }

    // Ambient lighting - FIXED
    vec3 kS = fresnelSchlick(max(dot(N, V), 0.0), F0);
    vec3 kD = (1.0 - kS) * (1.0 - tp.metallic);

    // Better ambient calculation with roughness consideration
    float ambientOcclusion = tp.ao;
    vec3 ambient = kD * tp.albedo * GetGlobalAmbient() * ambientOcclusion;

    vec3 color = ambient + Lo;

    // Optional HDR tonemapping
    // color = color / (color + vec3(1.0));

    // Gamma correction - make sure this is correct for your setup
    color = pow(color, vec3(1.0/2.2));  // Simple gamma correction

    // Or use your existing function if it's correct
    // color = Convert_LinearSpace_to_sRGB(color);

    FragColor = vec4(color, 1.0);
}
