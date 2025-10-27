#ifndef LIGHTING_CALC_GLSL
#define LIGHTING_CALC_GLSL

vec3 CalcDirectionalLight(int lightIndex, float NdotL, float NdotH, vec3 texDiff, vec3 texSpec, vec3 globalAmbient, vec3 matColor) {
    // Ambient calc
    vec3 ambient = globalAmbient * matColor * texDiff;
    // Diffuse calc
    vec3 diffuse = NdotL * GetLightDiffuse(lightIndex) * texDiff;

    // specular calc
    vec3 specular = NdotH * GetLightSpecular(lightIndex);

    return clamp(ambient + diffuse + specular, 0.0, 1.0);
}

vec3 CalcSpotLight(int lightIndex, float NdotL, float NdotH, vec3 FtoLight, float distance, vec3 texDiff, vec3 texSpec, vec3 globalAmbient, vec3 matColor) {
    // Ambient calc
    vec3 ambient = texDiff * globalAmbient * matColor;

    vec3 diffuse, specular = vec3(0.0);

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
        specular = NdotH * GetLightSpecular(lightIndex) * texSpec * attenuation * intensity;
    }

    return clamp(ambient + diffuse + specular, 0.0, 1.0);
}

vec3 CalcPointLight(int lightIndex, float NdotL, float NdotH, float distance, vec3 texDiff, vec3 texSpec, vec3 globalAmbient, vec3 matColor) {
    float attenuation = 1.0 / (GetLightConstant(lightIndex) + GetLightLinear(lightIndex) * distance +
                        GetLightQuadratic(lightIndex) * (distance * distance));

    // Ambient calc
    vec3 ambient = texDiff * globalAmbient * matColor;

    // Diffuse calc
    vec3 diffuse = NdotL * GetLightDiffuse(lightIndex) * texDiff * attenuation;

    // specular calc
    vec3 specular = NdotH * GetLightSpecular(lightIndex) * texSpec * attenuation;

    return clamp(ambient + diffuse + specular, 0.0, 1.0);
}

vec3 CalculateLighting(int lightIndex, vec3 normal, float NdotH, vec3 fragPos, vec3 texDiff, vec3 texSpec, vec3 globalAmbient, vec3 matColor) {
    int type = GetLightType(lightIndex);

    // Convert textures to Linear space
    texDiff = Convert_sRGB_to_LinearSpace(texDiff);
    texSpec = Convert_sRGB_to_LinearSpace(texSpec);

    if (type == 0) {
        vec3 FtoLight = GetLightPos(lightIndex) - fragPos;
        float distanceLength = length(FtoLight);
        vec3 lightDir = normalize(FtoLight);
        float NdotL = max(dot(normal, lightDir), 0.0);
        return CalcPointLight(lightIndex, NdotL, NdotH, distanceLength, texDiff, texSpec, globalAmbient, matColor);
    }
    else if (type == 1) {
        vec3 lightDir = GetLightDir(lightIndex);
        float NdotL = max(dot(normal, lightDir), 0.0);
        return CalcDirectionalLight(lightIndex, NdotL, NdotH, texDiff, texSpec, globalAmbient, matColor);
    }
    else if (type == 2) {
        vec3 lightPos = GetLightPos(lightIndex);
        vec3 FtoLight = normalize(lightPos - fragPos);
        float NdotL = max(dot(normal, FtoLight), 0.0);
        float distance = length(lightPos - fragPos);
        return CalcSpotLight(lightIndex, NdotL, NdotH, FtoLight, distance, texDiff, texSpec, globalAmbient, matColor);
    }
    return vec3(1.0, 0.0, 0.0); // red color for debugging
}

#endif