#version 460
#extension GL_ARB_bindless_texture : require
#extension GL_ARB_gpu_shader_int64 : enable

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

    vec3 albedo = GetAlbedoSampler2D(fs_in.MaterialIndex, fs_in.UV);
    float roughness = GetRoughnessSampler2D(fs_in.MaterialIndex, fs_in.UV);
    float metallic = GetMetallicSampler2D(fs_in.MaterialIndex, fs_in.UV);
    float ao = GetAOSampler2D(fs_in.MaterialIndex, fs_in.UV);

    vec3 N = GetNormalFromMap(GetNormalSampler2D(fs_in.MaterialIndex, fs_in.UV), fs_in.FragPos, fs_in.UV);
    vec3 V = normalize(GetViewPos() - fs_in.FragPos);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);
    for (int i = 0; i < lightCount; i++) {
        // calculate per-light radiance
        vec3 L = normalize(GetLightPos(i) - fs_in.FragPos);
        vec3 H = normalize(V + L);
        float distance = length(GetLightPos(i) - fs_in.FragPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = GetLightDiffuse(i) * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G   = GeometrySmith(N, V, L, roughness);
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;

        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }

    // ambient lighting (note that the next IBL tutorial will replace
    // this ambient lighting with environment lighting).
    vec3 ambient = GetGlobalAmbient() * albedo * ao;

    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/gamma));

    FragColor = vec4(albedo, 1.0);
}
