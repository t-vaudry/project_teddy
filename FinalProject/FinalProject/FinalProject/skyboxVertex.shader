#version 330 core

layout (location = 0) in vec3 position;

out vec3 skyCoord;
uniform mat4 mvp_matrix;
void main()
{
gl_Position = mvp_matrix * vec4(position, 1.0);
skyCoord = position;
}