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

    vec3 N  = GetNormalFromMap(GetNormalSampler2D(fs_in.MaterialIndex, fs_in.UV), fs_in.FragPos, normal, fs_in.UV);
    vec3 V  = normalize(GetViewPos() - fs_in.FragPos);
    vec3 F0 = mix(vec3(0.04), tp.albedo, tp.metallic);

    vec3 Lo = vec3(0.0);
    int lightCount = GetLightCount();
    for (int i = 0; i < lightCount; i++) {
        Lo += CalculatePBR(lights[i], pvd, N, F0, tp);
    }

    // TODO: Apply IBL ambient implementation after learning cubemaps and maybe HDR
    /*
        ambient lighting (note that the next IBL tutorial will replace
        this ambient lighting with environment lighting).
    */
    vec3 kS = fresnelSchlick(max(dot(N, V), 0.0), F0);
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - tp.metallic;
    vec3 ambient = (kD * tp.albedo) * GetGlobalAmbient() * tp.ao;
    vec3 color = ambient + Lo;

    // HDR tonemapping
    // color = color / (color + vec3(1.0));

    // gamma correct
    color = Convert_LinearSpace_to_sRGB(color);

    FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
