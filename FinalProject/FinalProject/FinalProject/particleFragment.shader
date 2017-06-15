#version 330 core

in vec4 particleColor;
in vec2 particleUV;

out vec4 color;

uniform sampler2D tex;

void main()
{
    color = (texture(tex, particleUV) * particleColor);
} 