#ifndef LIGHTING_CALC_GLSL
#define LIGHTING_CALC_GLSL
/*
    TODO: THE BIG PROBLEM HERE, REMOVE THESE FRAGMENT SHADER'S FUNCTIONS!
            AND SOLVE THE PROBLEM OF PREPROCESSOR !!!
*/

struct PerVertexData {
    vec3 fragPos;
    int materialIndex;
    vec3 normal;
    vec2 UV;
};

vec3 GetNormalFromMap(vec3 normalMap, vec3 fragPos, vec3 vertexNormal, vec2 TexCoords)
{
    vec3 tangentNormal = normalMap * 2.0 - 1.0;

    vec3 Q1  = dFdx(fragPos);
    vec3 Q2  = dFdy(fragPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(vertexNormal);  // Use vertex normal
    vec3 T   = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B   = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

float DistributionGGX(float NdotH, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(float NdotV, float NdotL, float roughness)
{
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 PrepareCommonPBRData(Light light, PerVertexData pvd, vec3 N, vec3 V, vec3 L, vec3 F0, TexturePack tp)
{
    // Unpacking texture packed
    vec3 albedo     = tp.albedo;
    float roughness = tp.roughness;
    float metallic  = tp.metallic;

    vec3 lightPos = light.pos_cutoff.xyz;
    vec3 radiance = light.radiance.xyz;

    vec3 H = normalize(V + L);
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float NdotH = max(dot(N, H), 0.0);
    float HdotV = max(dot(H, V), 0.0);

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(NdotH, roughness);
    float G   = GeometrySmith(NdotV, NdotL, roughness);
    vec3  F   = fresnelSchlick(HdotV, F0);

    vec3  numerator    = NDF * G * F;
    float denominator  = 4.0 * NdotV * NdotL + 0.0001; // + 0.0001 to prevent divide by zero
    vec3  specular     = numerator / denominator;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;

    kD *= 1.0 - metallic;

    return (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
}

vec3 CalculatePBR(Light light, PerVertexData pvd, vec3 N, vec3 F0, TexturePack tp)
{
    // Unpacking Per vertex data
    vec3 fragPos = pvd.fragPos;
    vec2 UV      = pvd.UV;
    int  matIdx  = pvd.materialIndex;

    vec3 lightPos = light.pos_cutoff.xyz;
    vec3 lightDir = light.dir_outer.xyz;

    vec3 V = normalize(GetViewPos() - fragPos);

    if (light.type == 0) { // POINT
        vec3 L = normalize(lightPos - fragPos);

        float distance = length(lightPos - fragPos);
        float attenuation = 1.0 / (light.constant +
                                   light.linear * distance +
                                   light.quadratic * (distance * distance)
        );

        return PrepareCommonPBRData(light, pvd, N, V, L, F0, tp) * attenuation;
    }
    else if (light.type == 1) { // DIRECTIONAL
        vec3 L = normalize(-lightDir);

        return PrepareCommonPBRData(light, pvd, N, V, L, F0, tp);
    }
    else if (light.type == 2) { // SPOT
        vec3 L = normalize(lightPos - fragPos);

        float cutOff      = light.pos_cutoff.w;
        float outerCutOff = light.dir_outer.w;

        float theta     = dot(L, normalize(-lightDir));
        float epsilon   = cutOff - outerCutOff;
        float intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);

        float distance = length(lightPos - fragPos);
        float attenuation = 1.0 / (light.constant +
                                   light.linear * distance +
                                   light.quadratic * (distance * distance)
        );

        return PrepareCommonPBRData(light, pvd, N, V, L, F0, tp) * attenuation * intensity;
    }

    // error case
    return vec3(0.0);
}

#endif
