#version 430 core

out vec4 FragColor;

layout (std140, binding = 0) uniform Camera
{
    mat4 projection;
    mat4 view;
    vec3 viewPos;
};

struct DirectionalLight
{
	vec3 direction;
    vec4 color; // rgb - color; a - intensity
    vec4 ambient; // rgb - color; a - intensity
};

layout (std140, binding = 3) uniform Directional
{
    DirectionalLight directionalLight;
};

uniform sampler2D directionalShadowMap;
uniform sampler2D diffuse;
uniform sampler2D specular;

in vec3 FragPos;
in vec3 Normal;
in vec2 Texcoord;
in vec4 FragPosLightSpace;

vec3 calcDirectionalLight(DirectionalLight directionalLight, vec3 norm, vec3 viewDir);
float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir);

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

	FragColor = vec4(result, 0.5); // vec4(viewDir, 0.5f);//
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    //float closestDepth = texture(directionalShadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
	//float bias = max(0.0005 * (1.0 - dot(Normal, lightDir)), 0.0005);
	float bias = 0.0005;
    //float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(directionalShadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(directionalShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 9.0;

	if(projCoords.z > 1.0)
        shadow = 0.0;

	return shadow;
}

vec3 calcDirectionalLight(DirectionalLight directionalLight, vec3 norm, vec3 viewDir)
{
	// Ambient
	vec3 directionalAmbient = (directionalLight.ambient.rgb * texture(diffuse, Texcoord).rgb) * directionalLight.ambient.w;

	// Diffuse
	vec3 lightDir = normalize(-directionalLight.direction.xyz);
	float diff = max(dot(norm, lightDir), 0.0f);
	vec3 directionalDiffuse = (directionalLight.color.rgb * diff * texture(diffuse, Texcoord).rgb) * directionalLight.color.w;

	// Specular
	vec3 reflectDir = reflect(-lightDir, norm);
	// TODO: Temp 0.5f shininess!!!!!!!!!
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 4.0);
	vec3 directionalSpecular = (directionalLight.color.rgb * spec * texture(specular, Texcoord).rgb) * directionalLight.color.w;

	// Calc shadow
	float shadow = ShadowCalculation(FragPosLightSpace, lightDir);

	return directionalAmbient + (1.0 - shadow) * (directionalDiffuse + directionalSpecular);
}