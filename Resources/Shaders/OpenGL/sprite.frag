#version 330 core

in vec2 UV;

out vec4 FragColor;

uniform sampler2D image;
uniform vec4 color;

void main()
{
    FragColor = texture(image, UV) * color;
}
