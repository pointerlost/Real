#version 460
#include "opengl/buffers.glsl"

in vec3 vFragPos;
in vec3 vNormal;
in vec2 vUV;
flat in int vMaterialIndex;

uniform vec3 g_GlobalAmbient;
uniform int uLightCount;

out vec4 FragColor;
float specularStrength = 0.5;

void main() {
    vec3 result = vec3(0.0);
    vec3 normal = normalize(vNormal);

    for (int i = 0; i < uLightCount; i++) {
        vec3 lightPos = GetLightPos(i);

        // Point light
        float distance = length(lightPos - vFragPos);
        float attenuation = 1.0 / (GetLightConstant(i) + GetLightLinear(i) * distance +
                            GetLightQuadratic(i) * (distance * distance));

        vec3 lightDir = normalize(lightPos - vFragPos);
        float diff = max(dot(normal, lightDir), 0.0);

        vec3 diffuse = (diff * GetDiffuse(i));
        diffuse *= attenuation;

        // specular
        vec3 viewDir = normalize(GetViewPos() - vFragPos);
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
        vec3 specular = specularStrength * spec * GetSpecular(i);
        specular *= attenuation;

        result += (diffuse + specular);
    }
    result += g_GlobalAmbient;
    FragColor = vec4(result, 1.0) * GetBaseColor(vMaterialIndex) * texture(u_TextureArray, vec3(vUV, GetTextureLayer(vMaterialIndex)));
}
/*
float distance    = length(light.position - FragPos);
float attenuation = 1.0 / (light.constant + light.linear * distance +
    		    light.quadratic * (distance * distance));
*/

// FragColor = vec4(ambient + diffuse + specular, 1.0) * texture2D(tex, vUV);