#version 330 core

in vec3 vertexColor;
in vec3 vertexNormal;
in vec3 vertexPosition;

out vec4 color;

uniform vec3 ambientLight;
uniform vec3 diffuseLight;
uniform vec3 spotLight;
uniform vec3 sunLight;
uniform vec3 cameraPosition;

uniform float constantFactor;
uniform float linearFactor;
uniform float quadraticFactor;

uniform bool lightOn;

void main()
{
    // Diffuse light
    vec3 diffuseLightVector = normalize(diffuseLight - vertexPosition);
    float distance = distance(diffuseLight, vertexPosition);
    float diffuse = dot(diffuseLightVector, normalize(vertexNormal));
    float diffuseAttenuation = 1.0f / (constantFactor + linearFactor * distance + quadraticFactor * distance * distance);

    // Spot light
    vec3 spotLightVector = normalize(spotLight - vertexPosition);
    distance = distance(spotLight, vertexPosition);
    float spot = dot(spotLightVector, normalize(vertexNormal));
    float spotAttenuation = 1.0f / (constantFactor + linearFactor * distance + quadraticFactor * distance * distance);

    // Sun light
    vec3 sunLightVector = normalize(sunLight - vertexPosition);
    distance = distance(sunLight, vertexPosition);
    float sun = dot(sunLightVector, normalize(vertexNormal));
    float sunAttenuation = 0.5f;

    // Specular light
    vec3 reflectedLightVector = normalize(reflect(-diffuseLightVector, normalize(vertexNormal)));
    vec3 cameraVector = normalize(cameraPosition - vertexPosition);
    float specular = pow(clamp(dot(reflectedLightVector, cameraVector), 0, 1), 50);

    vec4 allLight;

    if (lightOn)
        allLight = vec4(ambientLight + sunAttenuation * vec3(clamp(sun, 0, 1)) + spotAttenuation * vec3(clamp(spot, 0, 1)) + (vec3(clamp(diffuse, 0, 1)) + vec3(specular)), 1.0f);
    else
        allLight = vec4(1.0f);

    color = allLight * vec4(vertexColor, 1.0f) ;
} 