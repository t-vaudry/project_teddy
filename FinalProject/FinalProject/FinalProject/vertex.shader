#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec2 uv;

out vec3 vertexPosition;
out vec3 vertexColor;
out vec3 vertexNormal;
out vec2 vertexUV;

uniform mat4 mvp_matrix;
uniform mat4 modelMatrix;

void main()
{
    gl_Position = mvp_matrix * vec4(position, 1.0);
    vertexPosition = vec3(modelMatrix * vec4(position, 1.0f));
    vertexColor = color;
    vertexNormal = mat3(modelMatrix) * normal;
    vertexUV = uv;
}