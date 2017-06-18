#version 330 core

in vec3 vertexNormal;
in vec3 vertexPosition;
in vec2 vertexUV;

out vec4 color;
uniform samplerCube depth;
uniform float far_plane;
uniform vec3 light_position;
void main()
{
	
    vec3 lightColour = vec3(1.0f, 1.0f, 1.0f);
	vec3 cubeColour = vec3(1, 0, 1);
	float ambientStrength = 0.15f;
    vec3 ambient_contribution = ambientStrength * lightColour;
	
	//diffuse lighting

	vec3 norm = normalize(vertexNormal);
	vec3 light_vector = (light_position - vertexPosition);
	vec3 light_direction = normalize(light_vector);
	float light_distance = length(light_vector);
	float incident_degree = 1.0f;
	if (light_distance > 0.0f) {
		incident_degree = max(dot(norm, light_direction), 0.0f);
	}
	vec3 diffuse_contribution = incident_degree * lightColour;
	

	float closestDepth = texture(depth, -light_vector).r;
	closestDepth *= far_plane;
	float bias = 0.1; 
	float shadow = light_distance - bias > closestDepth ? 1.0 : 0.0;

	vec3 resultantColour = (ambient_contribution + (diffuse_contribution*(1-shadow))) * cubeColour;
    color = vec4(resultantColour, 1.0f);
    //color = vec4(shadow, shadow, shadow, 1.0f);
} 