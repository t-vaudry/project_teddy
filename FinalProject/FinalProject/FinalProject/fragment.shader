#version 330 core

in vec3 vertexNormal;
in vec3 vertexColor;
in vec3 vertexPosition;
in vec2 vertexUV;

out vec4 color;

void main()
{
    color = vec4(vertexColor, 1.0f);
} 