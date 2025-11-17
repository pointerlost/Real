#ifndef UTILS_GLSL
#define UTILS_GLSL

const float gamma = 2.2;
const float PI = 3.14159265359;

// TODO: Converting algorithm can improve!
vec3 Convert_sRGB_to_LinearSpace(vec3 content) {
    return pow(content, vec3(gamma));
}

vec3 Convert_LinearSpace_to_sRGB(vec3 content) {
    return pow(content, vec3(1.0 / gamma));
}

#endif