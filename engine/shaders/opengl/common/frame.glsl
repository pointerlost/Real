#ifndef FRAME_GLSL
#define FRAME_GLSL

layout(std140, binding = 3) uniform FrameUBO {
    vec4 position;
    mat4 view;
    mat4 projection;
    mat4 viewProjection;
} uFrame;

vec3 GetViewPos()    { return uFrame.position.xyz;   }
mat4 GetProjView()   { return uFrame.viewProjection; }
mat4 GetView()       { return uFrame.view;           }
mat4 GetProjection() { return uFrame.projection;     }

#endif