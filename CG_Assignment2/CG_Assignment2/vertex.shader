
#version 330 core
  
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 normal;

out vec3 vertexColor;

uniform mat4 mvp_matrix;

void main()
{
    gl_Position = mvp_matrix * vec4(position.x, position.y, position.z, 1.0f);
    vertexColor = color;
}