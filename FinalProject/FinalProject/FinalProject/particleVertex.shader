
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 uv;
layout (location = 3) in float alpha;

out vec4 particleColor;
out vec2 particleUV;

uniform mat4 vp_matrix;

void main()
{
    gl_Position = vp_matrix * vec4(position, 1.0);
    particleColor = vec4(color, alpha);
    particleUV = uv;
}