#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform CameraBufferObject {
    mat4 view;
    mat4 proj;
} cbo;

layout(push_constant) uniform PushConstant {
    mat4 model;
} pushConstant;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;

layout(location = 0) out vec2 fragUV;

void main() {
    gl_Position = cbo.proj * cbo.view * pushConstant.model * vec4(inPosition, 1.0);
    fragUV = inUV;
}
