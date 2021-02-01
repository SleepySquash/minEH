#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 fragUV;
layout(binding = 0) uniform sampler2D tex;

layout(push_constant) uniform PushConstant {
    layout(offset = 64) vec4 color;
} pushConstant;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = texture(tex, fragUV) * pushConstant.color;
}
