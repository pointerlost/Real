#version 460

in vec3 vFragPos;
in vec3 vNormal;
in vec2 vUV;

uniform sampler2D tex;

uniform vec3 g_GlobalAmbient;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

out vec4 FragColor;

float specularStrength = 0.5f;

void main() {
    vec3 normal = normalize(vNormal);
    vec3 lightDir = normalize(lightPos - vFragPos);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 ambient = g_GlobalAmbient * lightColor;

    vec3 diffuse = diff * lightColor;

    // specular
    vec3 viewDir = normalize(viewPos - vFragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    FragColor = vec4(ambient + diffuse + specular, 1.0) * texture2D(tex, vUV);
}