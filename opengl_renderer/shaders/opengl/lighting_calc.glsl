#ifndef LIGHTING_CALC_GLSL
#define LIGHTING_CALC_GLSL

float specularStrength = 0.5;

vec3 CalcDirectionalLight(int lightIndex, vec3 viewDir, vec3 normal) {
    vec3 lightDir = normalize(-GetLightDir(lightIndex)); // Negate because light points toward scene

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = (diff * GetLightDiffuse(lightIndex));

    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * GetLightSpecular(lightIndex);

    return (diffuse + specular);
}


vec3 CalcSpotLight(int lightIndex, vec3 fragPos, vec3 viewDir, vec3 normal) {
    vec3 lightPos = GetLightPos(lightIndex);

    vec3 lightDir = normalize(lightPos - fragPos); // direction from light to fragment
    vec3 lightForward = normalize(-GetLightDir(lightIndex));

    // Calc angle between light forward and fragment direction
    float theta = dot(lightDir, lightForward);
    float epsilon = GetCutOff(lightIndex) - GetOuterCutOff(lightIndex);
    float intensity = clamp((theta - GetOuterCutOff(lightIndex)) / epsilon, 0.0, 1.0);

    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    if (theta > GetOuterCutOff(lightIndex)) {
        float distance = length(GetLightPos(lightIndex) - fragPos);
        float attenuation = 1.0 / (GetLightConstant(lightIndex) + GetLightLinear(lightIndex) * distance +
                        GetLightQuadratic(lightIndex) * (distance * distance));

        // Diffuse
        float diff = max(dot(normal, lightDir), 0.0);
        diffuse = (diff * GetLightDiffuse(lightIndex));
        diffuse *= attenuation * intensity;

        // Specular
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
        specular = specularStrength * spec * GetLightSpecular(lightIndex);
        specular *= attenuation * intensity;
    }

    return (diffuse + specular);
}


vec3 CalcPointLight(int lightIndex, vec3 fragPos, vec3 viewDir, vec3 normal) {
    vec3 lightPos = GetLightPos(lightIndex);
    vec3 lightDir = normalize(lightPos - fragPos);

    float distance = length(lightPos - fragPos);
    float attenuation = 1.0 / (GetLightConstant(lightIndex) + GetLightLinear(lightIndex) * distance +
                        GetLightQuadratic(lightIndex) * (distance * distance));

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = (diff * GetLightDiffuse(lightIndex));
    diffuse *= attenuation;

    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * GetLightSpecular(lightIndex);
    specular *= attenuation;

    return (diffuse + specular);
}

vec3 CalculateLighting(int lightIndex, vec3 fragPos, vec3 viewDir, vec3 normal) {
    int type = GetLightType(lightIndex);
    if (type == 0) {
        return CalcPointLight(lightIndex, fragPos, viewDir, normal);
    }
    else if (type == 1) {
        return CalcDirectionalLight(lightIndex, viewDir, normal);
    }
    else if (type == 2) {
        return CalcSpotLight(lightIndex, fragPos, viewDir, normal);
    }
    return vec3(0.0);
}

#endif