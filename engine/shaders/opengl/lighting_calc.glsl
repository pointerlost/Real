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

    // Proper TBN matrix calculation
    vec3 N = normalize(vertexNormal);
    vec3 T = Q1 * st2.t - Q2 * st1.t;
    vec3 B = Q2 * st1.s - Q1 * st2.s;

    float invLen = inversesqrt(max(dot(T, T), dot(B, B)));
    T *= invLen;
    B *= invLen;

    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

float DistributionGGX(float NdotH, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH2 = NdotH * NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / max(denom, 0.000001);  // Prevent division by zero
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    // Direct lighting formula (different from IBL)
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / max(denom, 0.000001);
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
    vec3  albedo    = tp.albedo;
    float roughness = tp.roughness;
    float metallic  = tp.metallic;

    vec3 radiance = light.radiance.xyz;

    vec3 H = normalize(V + L);
    float NdotV = max(dot(N, V), 0.000001);  // Prevent division by zero
    float NdotL = max(dot(N, L), 0.000001);
    float NdotH = max(dot(N, H), 0.000001);
    float VdotH = max(dot(V, H), 0.000001);

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(NdotH, roughness);
    float G   = GeometrySmith(NdotV, NdotL, roughness);
    vec3  F   = fresnelSchlick(VdotH, F0);  // Use V·H not H·V

    vec3  numerator    = NDF * G * F;
    float denominator  = 4.0 * NdotV * NdotL;
    vec3  specular     = numerator / max(denominator, 0.000001);

    // Energy conservation
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;

    // Metals have no diffuse reflection
    kD *= 1.0 - metallic;

    // Final composition
    vec3 diffuse  = kD * albedo / PI;
    vec3 brdf     = diffuse + specular;

    return brdf * radiance * NdotL;
}

vec3 CalculatePBR(Light light, PerVertexData pvd, vec3 N, vec3 F0, TexturePack tp)
{
    // Unpacking Per vertex data
    vec3 fragPos  = pvd.fragPos;
    vec3 lightPos = light.pos_cutoff.xyz;
    vec3 lightDir = light.dir_outer.xyz;

    vec3 V = normalize(GetViewPos() - fragPos);

    if (light.type == 0) { // POINT
        vec3 L = normalize(lightPos - fragPos);
        float distance = length(lightPos - fragPos);

        // Range check
        if (distance > light.dir_outer.w) {
            return vec3(0.0);
        }

        // Attenuation
        float attenuation = 1.0 / (light.constant +
                                   light.linear * distance +
                                   light.quadratic * (distance * distance));

        return PrepareCommonPBRData(light, pvd, N, V, L, F0, tp) * attenuation;
    }
    else if (light.type == 1) { // DIRECTIONAL
        vec3 L = normalize(-lightDir);
        return PrepareCommonPBRData(light, pvd, N, V, L, F0, tp);
    }
    else if (light.type == 2) { // SPOT
        vec3 L = normalize(lightPos - fragPos);
        vec3 lightDirNorm = normalize(lightDir);

        float cutOff      = cos(radians(light.pos_cutoff.w));      // Inner cutoff
        float outerCutOff = cos(radians(light.dir_outer.w));       // Outer cutoff

        float theta = dot(-L, lightDirNorm);  // Angle between light direction and fragment
        float epsilon = cutOff - outerCutOff;
        float intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);

        // Early exit if outside cone
        if (theta < outerCutOff) {
            return vec3(0.0);
        }

        float distance = length(lightPos - fragPos);
        float attenuation = 1.0 / (light.constant +
                                   light.linear * distance +
                                   light.quadratic * (distance * distance));

        return PrepareCommonPBRData(light, pvd, N, V, L, F0, tp) * attenuation * intensity;
    }

    // error case
    return vec3(0.0);
}

#endif
