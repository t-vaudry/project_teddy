#version 330 core

in vec3 vertexPosition;
in vec3 vertexColor;
in vec3 vertexNormal;
in vec2 vertexUV;

out vec4 color;

uniform float farPlane;
uniform float alpha;

uniform vec3 ambientLight;
uniform vec3 sunLight;

uniform float constantFactor;
uniform float linearFactor;
uniform float quadraticFactor;

void main()
{
    // Sun light
    vec3 sunLightVector = normalize(sunLight - vertexPosition);
    float distance = distance(sunLight, vertexPosition);
    float sun = dot(sunLightVector, normalize(vertexNormal));
    float sunAttenuation = 1.0f / (constantFactor + linearFactor * distance + quadraticFactor * distance * distance);

    vec4 allLight = vec4(ambientLight + clamp(sunAttenuation, 0, 1) * vec3(clamp(sun, 0, 1)), 1.0f);

    color = allLight * vec4(vertexColor, alpha);
} 