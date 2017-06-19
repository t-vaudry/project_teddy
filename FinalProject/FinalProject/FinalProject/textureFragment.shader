#version 330 core

in vec3 vertexPosition;
in vec3 vertexColor;
in vec3 vertexNormal;
in vec2 vertexUV;

out vec4 color;

uniform float alpha;
uniform sampler2D textureSample;

uniform samplerCube depth;
uniform samplerCube depth2;
uniform samplerCube depth3;

uniform vec3 ambientLight;
uniform vec3 sunLight;
uniform vec3 lightSwitch;
uniform vec3 lightIntensity;

uniform float far_plane;
uniform vec3 light_position;

uniform float constantFactor;
uniform float linearFactor;
uniform float quadraticFactor;

uniform bool invalidPosition;
uniform bool noLight;

void main()
{
    // Sun light
    /*
    vec3 sunLightVector = normalize(sunLight - vertexPosition);
    float distance = distance(sunLight, vertexPosition);
    float sun = max(dot(sunLightVector, normalize(vertexNormal)), 0.0f);
    float sunAttenuation = 1.0f / (constantFactor + linearFactor * distance + quadraticFactor * distance * distance);
    */

    //Room1
    vec3 room1Pos = vec3(28.0f, 0.0f, 28.0f);
    vec3 room1Vector = normalize(room1Pos - vertexPosition);
    float distance = distance(room1Pos, vertexPosition);
    float room1Light = max(dot(room1Vector, normalize(vertexNormal)), 0.0f);
    float room1AttenuationLight = 1.0f / (constantFactor + linearFactor * distance + quadraticFactor * distance * distance);

    float closestDepth = texture(depth, -room1Vector).r;
	closestDepth *= far_plane;
	float bias = 0.1; 
	float shadow1 = distance - bias > closestDepth ? 1.0 : 0.0;

    //Room2
    vec3 room2Pos = vec3(33.0f, 0.0f, 28.0f);
    vec3 room2Vector = normalize(room2Pos - vertexPosition);
    distance = distance(room2Pos, vertexPosition);
    float room2Light = max(dot(room2Vector, normalize(vertexNormal)), 0.0f);
    float room2AttenuationLight = 1.0f / (constantFactor + linearFactor * distance + quadraticFactor * distance * distance);

    closestDepth = texture(depth2, -room2Vector).r;
	closestDepth *= far_plane;
	float shadow2 = distance - bias > closestDepth ? 1.0 : 0.0;

    //Room3
    vec3 room3Pos = vec3(28.0f, 0.0f, 33.0f);
    vec3 room3Vector = normalize(room3Pos - vertexPosition);
    distance = distance(room3Pos, vertexPosition);
    float room3Light = max(dot(room3Vector, normalize(vertexNormal)), 0.0f);
    float room3AttenuationLight = 1.0f / (constantFactor + linearFactor * distance + quadraticFactor * distance * distance);

    closestDepth = texture(depth3, -room3Vector).r;
	closestDepth *= far_plane;
	float shadow3 = distance - bias > closestDepth ? 1.0 : 0.0;

    vec4 room1;
    if (lightSwitch.x == 1.0f)
        room1 = vec4(room1AttenuationLight * vec3(clamp(room1Light, 0, 1)), 1.0f) * (1-shadow1) * ((lightIntensity.x + 1.0f)/2.0f);
    else
        room1 = vec4(0.0f, 0.0f, 0.0f, 1.0f);

    vec4 room2;
    if (lightSwitch.y == 1.0f)
        room2 = vec4(room2AttenuationLight * vec3(clamp(room2Light, 0, 1)), 1.0f) * (1 - shadow2) * ((lightIntensity.y + 1.0f)/2.0f);
    else
        room2 = vec4(0.0f, 0.0f, 0.0f, 1.0f);

    vec4 room3;
    if (lightSwitch.z == 1.0f)
        room3 = vec4(room3AttenuationLight * vec3(clamp(room3Light, 0, 1)), 1.0f) * (1 - shadow3) * ((lightIntensity.z + 1.0f)/2.0f);
    else
        room3 = vec4(0.0f, 0.0f, 0.0f, 1.0f);

    vec4 allLight = room1 + room2 + room3 + vec4(0.1f, 0.1f, 0.1f, 1.0f);

    if (invalidPosition)
        color = vec4(1.0f, 0.0f, 0.0f, 1.0f) * vec4(texture(textureSample, vertexUV).rgb, alpha);
    else if (noLight)
        color = vec4(texture(textureSample, vertexUV).rgb, alpha);
    else
        color = vec4(allLight.xyz, 1.0f) * vec4(texture(textureSample, vertexUV).rgb, alpha);
}