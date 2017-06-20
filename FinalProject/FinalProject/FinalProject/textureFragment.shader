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
uniform samplerCube depthSun;

uniform vec3 room1LightPosition;
uniform vec3 room2LightPosition;
uniform vec3 room3LightPosition;

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
    //Room1
    vec3 room1Pos = room1LightPosition;
    vec3 room1Vector = normalize(room1Pos - vertexPosition);
    float distance = distance(room1Pos, vertexPosition);
    float room1Light = max(dot(room1Vector, normalize(vertexNormal)), 0.0f);
    float room1AttenuationLight = 1.0f / (constantFactor + linearFactor * distance + quadraticFactor * distance * distance);

    // Determine shadows from Room1 light
    float closestDepth = texture(depth, -room1Vector).r;
    closestDepth *= far_plane;
    float bias = 0.15; 
    float shadow1 = distance - bias > closestDepth ? 1.0 : 0.0;

    //Room2
    vec3 room2Pos = room2LightPosition;
    vec3 room2Vector = normalize(room2Pos - vertexPosition);
    distance = distance(room2Pos, vertexPosition);
    float room2Light = max(dot(room2Vector, normalize(vertexNormal)), 0.0f);
    float room2AttenuationLight = 1.0f / (constantFactor + linearFactor * distance + quadraticFactor * distance * distance);

    // Determine shadows from Room2 light
    closestDepth = texture(depth2, -room2Vector).r;
    closestDepth *= far_plane;
    float shadow2 = distance - bias > closestDepth ? 1.0 : 0.0;

    //Room3
    vec3 room3Pos = room3LightPosition;
    vec3 room3Vector = normalize(room3Pos - vertexPosition);
    distance = distance(room3Pos, vertexPosition);
    float room3Light = max(dot(room3Vector, normalize(vertexNormal)), 0.0f);
    float room3AttenuationLight = 1.0f / (constantFactor + linearFactor * distance + quadraticFactor * distance * distance);

    // Determine shadows from Room3 light
    closestDepth = texture(depth3, -room3Vector).r;
    closestDepth *= far_plane;
    float shadow3 = distance - bias > closestDepth ? 1.0 : 0.0;

    vec4 room1;
    // If light is on
    if (lightSwitch.x == 1.0f)
        room1 = vec4(room1AttenuationLight * vec3(clamp(room1Light, 0, 1)), 1.0f) * (1-shadow1) * ((lightIntensity.x + 1.0f)/2.0f);
    else
        room1 = vec4(0.0f, 0.0f, 0.0f, 1.0f);

    vec4 room2;
    // If light is on
    if (lightSwitch.y == 1.0f)
        room2 = vec4(room2AttenuationLight * vec3(clamp(room2Light, 0, 1)), 1.0f) * (1 - shadow2) * ((lightIntensity.y + 1.0f)/2.0f);
    else
        room2 = vec4(0.0f, 0.0f, 0.0f, 1.0f);

    vec4 room3;
    // If light is on
    if (lightSwitch.z == 1.0f)
        room3 = vec4(room3AttenuationLight * vec3(clamp(room3Light, 0, 1)), 1.0f) * (1 - shadow3) * ((lightIntensity.z + 1.0f)/2.0f);
    else
        room3 = vec4(0.0f, 0.0f, 0.0f, 1.0f);

    //Sunlight
    vec3 sunlightPos = vec3(22.0f, 5.0f, 28.0f);
    vec3 sunlightVector = normalize(sunlightPos - vertexPosition);
    distance = distance(sunlightPos, vertexPosition);
    float sunlight = max(dot(sunlightVector, normalize(vertexNormal)), 0.0f);
    float sunlightAttenuation = 1.0f / (constantFactor + linearFactor * distance + quadraticFactor * distance * distance);

    // Determine shadows from sun light
    closestDepth = texture(depthSun, -sunlightVector).r;
    closestDepth *= far_plane;
    float shadowSun = distance - bias > closestDepth ? 1.0 : 0.0;
    vec4 sun = vec4(sunlightAttenuation * vec3(clamp(sunlight, 0, 1)), 1.0f) * (1-shadowSun);

    // Add all light together
    vec4 allLight = room1 + room2 + room3 + sun + vec4(0.1f, 0.1f, 0.1f, 1.0f);

    // Set red if invalid
    if (invalidPosition)
        color = vec4(1.0f, 0.0f, 0.0f, 1.0f) * vec4(texture(textureSample, vertexUV).rgb, alpha);
    else if (noLight) // For HUD
        color = vec4(texture(textureSample, vertexUV).rgb, alpha);
    else // Display with lighting
        color = vec4(allLight.xyz, 1.0f) * vec4(texture(textureSample, vertexUV).rgb, alpha);
}