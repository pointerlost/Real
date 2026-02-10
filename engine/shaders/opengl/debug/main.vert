#version 460 core
#extension GL_NV_gpu_shader5 : enable
#ifndef DEBUG_MAIN_VERTEX_SHADER
#define DEBUG_MAIN_VERTEX_SHADER

#include "opengl/common/frame.glsl"
#include "opengl/common/buffers.glsl"

layout (location = 0) in vec3 aPosition;

struct DebugInstance {
    mat4 model;
    vec4 color;
};

layout(std430, binding = 1) buffer DebugInstances {
    DebugInstance debugInstances[];
};

out vec4 vColor;

void main() {
    DebugInstance inst = debugInstances[gl_BaseInstance + gl_InstanceID];

    mat4 model = inst.model;
    vColor     = inst.color;

    gl_Position = GetProjView() * model * vec4(aPosition, 1.0);
}

#endif