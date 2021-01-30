#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inUV;

layout(push_constant) uniform PushConstant {
    mat4 model;
} pushConstant;

layout(location = 0) out vec2 fragUV;

void main() {
    gl_Position = pushConstant.model * vec4(inPosition, 0.0, 1.0);
    fragUV = inUV;
}
