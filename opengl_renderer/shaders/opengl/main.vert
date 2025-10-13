#version 460

// TODO: #extension GL_ARB_bindless_texture : require

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

out vec3 vFragPos;
out vec3 vNormal;
out vec2 vUV;

uniform mat3 u_NormalMatrix;
uniform mat4 ModelMat;
uniform mat4 View;
uniform mat4 Projection;

void main() {
    vFragPos = vec3(ModelMat * vec4(aPos, 1.0));
    vNormal = u_NormalMatrix * aNormal;
    vUV = aUV;
    gl_Position = Projection * View * ModelMat * vec4(aPos, 1.0f);
}