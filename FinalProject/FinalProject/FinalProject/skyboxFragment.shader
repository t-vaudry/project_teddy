#version 330 core

in vec3 skyCoord;

out vec4 color;

uniform samplerCube textureSample;

void main()
{
    color = texture(textureSample, skyCoord);
} 