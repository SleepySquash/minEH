#version 330 core

in vec2 UV;

out vec4 FragColor;

uniform sampler2D text;
uniform vec4 color;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, UV).r);
    FragColor = color * sampled;
}
