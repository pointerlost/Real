#ifndef LIGHTING_CALC_GLSL
#define LIGHTING_CALC_GLSL

vec3 CalcDirectionalLight(int lightIndex, float NdotL, float RdotV, vec3 texDiff, vec3 texSpec, vec3 globalAmbient, vec3 matColor) {

    vec3 ambient = globalAmbient * matColor * texDiff;
    // Diffuse calc
    vec3 diffuse = NdotL * GetLightDiffuse(lightIndex) * texDiff;

    // specular calc
    float spec = pow(RdotV, 32.0f);
    vec3 specular = spec * GetLightSpecular(lightIndex) * texSpec;

    return clamp(ambient + diffuse + specular, 0.0, 1.0);
}

vec3 CalcSpotLight(int lightIndex, float NdotL, float RdotV, vec3 FtoLight, float distance, vec3 texDiff, vec3 texSpec, vec3 globalAmbient, vec3 matColor) {
    vec3 diffuse, specular = vec3(0.0);

    vec3 ambient = texDiff * globalAmbient * matColor;

    // Calc angle between light forward and fragment direction
    float theta = dot(FtoLight, GetLightDir(lightIndex));
    float epsilon = GetCutOff(lightIndex) - GetOuterCutOff(lightIndex);
    float intensity = clamp((theta - GetOuterCutOff(lightIndex)) / epsilon, 0.0, 1.0);

    if (theta > GetOuterCutOff(lightIndex)) {
        float attenuation = 1.0 / (GetLightConstant(lightIndex) + GetLightLinear(lightIndex) * distance +
                        GetLightQuadratic(lightIndex) * (distance * distance));

        // Diffuse calc
        diffuse = NdotL * GetLightDiffuse(lightIndex) * texDiff * attenuation * intensity;

        // Specular calc
        float spec = pow(RdotV, 32.0f);
        specular = spec * GetLightSpecular(lightIndex) * texSpec * attenuation * intensity;
    }

    return clamp(ambient + diffuse + specular, 0.0, 1.0);
}

vec3 CalcPointLight(int lightIndex, float NdotL, float RdotV, float distance, vec3 texDiff, vec3 texSpec, vec3 globalAmbient, vec3 matColor) {
    float attenuation = 1.0 / (GetLightConstant(lightIndex) + GetLightLinear(lightIndex) * distance +
                        GetLightQuadratic(lightIndex) * (distance * distance));

    vec3 ambient = texDiff * globalAmbient * matColor;

    // Diffuse calc
    vec3 diffuse = NdotL * GetLightDiffuse(lightIndex) * texDiff * attenuation;

    // specular calc
    float spec = pow(RdotV, 32.0);
    vec3 specular = spec * GetLightSpecular(lightIndex) * texSpec * attenuation;

    return clamp(ambient + diffuse + specular, 0.0, 1.0);
}

vec3 CalculateLighting(int lightIndex, float NdotL, float RdotV, vec3 fragPos, vec3 texDiff, vec3 texSpec, vec3 globalAmbient, vec3 matColor) {
    int type = GetLightType(lightIndex);

    if (type == 0) {
        float distance = length(GetLightPos(lightIndex) - fragPos);
        return CalcPointLight(lightIndex, NdotL, RdotV, distance, texDiff, texSpec, globalAmbient, matColor);
    }
    else if (type == 1) {
        return CalcDirectionalLight(lightIndex, NdotL, RdotV, texDiff, texSpec, globalAmbient, matColor);
    }
    else if (type == 2) {
        vec3 lightPos = GetLightPos(lightIndex);
        vec3 FtoLight = normalize(lightPos - fragPos);
        float distance = length(lightPos - fragPos);
        return CalcSpotLight(lightIndex, NdotL, RdotV, FtoLight, distance, texDiff, texSpec, globalAmbient, matColor);
    }
    return vec3(1.0, 0.0, 0.0); // red color for debugging
}

#endif