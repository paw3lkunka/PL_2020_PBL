#version 430 core

out vec4 FragColor;

struct DirectionalLight
{
	vec3 direction;
    vec4 color; // rgb - color; a - intensity
    vec4 ambient; // rgb - color; a - intensity
};

layout (std140, binding = 3) uniform DirectionalLight
{
    DirectionalLight directionalLight;
};

uniform sampler2D diffuse;
uniform sampler2D specular;

in vec3 FragPos;
in vec3 Normal;
in vec2 Texcoord;

vec3 calcDirectionalLight(DirectionalLight directionalLight, vec3 norm, vec3 viewDir);

void main() 
{ 
// Normalized normal vector for use in light calculations

	// Global helper variables
	vec3 viewDir, norm, result;

	// Shared values
	result = vec3(0.0f, 0.0f, 0.0f);
	norm = normalize(Normal);
	viewDir = normalize(viewPos - FragPos);

// Directional light ------------------------------------------------------------------------
	result += calcDirectionalLight(directionalLight, norm, viewDir);

	FragColor = vec4(result, 1.0f);
}


vec3 calcDirectionalLight(DirectionalLight directionalLight, vec3 norm, vec3 viewDir)
{
	// Ambient
	vec3 directionalAmbient = directionalLight.ambient * texture(diffuse, Texcoord).rgb;

	// Diffuse
	vec3 lightDir = normalize(-directionalLight.direction);
	float diff = max(dot(norm, lightDir), 0.0f);
	vec3 directionalDiffuse = directionalLight.color * diff * texture(diffuse, Texcoord).rgb;

	// Specular
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
	vec3 directionalSpecular = directionalLight.color * spec * texture(specular, Texcoord).rgb;

	return directionalAmbient + directionalDiffuse + directionalSpecular;
}