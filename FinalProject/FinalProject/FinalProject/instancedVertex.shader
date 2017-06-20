#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec2 uv;
layout (location = 4) in mat4 modelMatrix;

out vec3 vertexPosition;
out vec3 vertexColor;
out vec3 vertexNormal;
out vec2 vertexUV;

uniform mat4 vp_matrix;

void main()
{
    gl_Position = vp_matrix * modelMatrix * vec4(position, 1.0f);
    vertexPosition = vec3(modelMatrix * vec4(position, 1.0f));
    vertexColor = color;
    vertexNormal = mat3(modelMatrix) * normal;
    vertexUV = uv;
}