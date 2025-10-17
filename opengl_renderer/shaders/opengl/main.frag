#version 460
// Before you include any file, pay attention to the order.
// if lighting calculations need to use buffers.glsl so you have to include lighting.glsl first!
#include "opengl/buffers.glsl"
#include "opengl/lighting_calc.glsl"

in vec3 vFragPos;
in vec3 vNormal;
in vec2 vUV;
flat in int vMaterialIndex;

uniform vec3 g_GlobalAmbient;
uniform int uLightCount;

out vec4 FragColor;

void main() {
    vec3 result = vec3(0.0);
    vec3 normal = normalize(vNormal);

    for (int i = 0; i < uLightCount; i++) {
        vec3 viewDir = normalize(GetViewPos() - vFragPos);

        result += CalculateLighting(i, vFragPos, viewDir, normal);
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