#version 460 core
#ifndef DEBUG_MAIN_VERTEX_SHADER
#define DEBUG_MAIN_VERTEX_SHADER

in vec4 vColor;
out vec4 FragColor;

void main() {
    FragColor = vColor;
}

#endif