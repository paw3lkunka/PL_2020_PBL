#version 430 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

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

layout (std140, binding = 5) uniform Time
{
    float _Time;
};

uniform vec2 uvScale = vec2(1.0, 1.0);
uniform sampler2D diffuse;
uniform sampler2D normal;
uniform sampler2D occRouMet; // r - occlusion, g - roughness, b - metallic

uniform sampler2D directionalShadowMap;
uniform samplerCube irradianceMap;

// ===== Flow map configuration =====
uniform sampler2D flowMap;
uniform vec2 channelFlowDirection = vec2(1.0, -1.0);
uniform float blendCycle = 1.0;
uniform float cycleSpeed = 1.0;
uniform float flowSpeed = 0.5;
uniform sampler2D flowMapNoise;
uniform vec2 flowNoiseSize = vec2(1.0, 1.0);
uniform float flowNoiseInfluence = 0.5;

in vec3 FragPos;
in vec3 Normal;
in vec2 Texcoord;
in mat3 TBN;
in vec4 FragPosLightSpace;

// Shader constants
const float PI = 3.14159265359;
const vec2 jitter[9] = {
	vec2(0.1, 0.2),
	vec2(-0.3, 0.15),
	vec2(-0.25, -0.3),
	vec2(0.05, -0.35),
	vec2(0.15, 0.2),
	vec2(0.12, 0.22),
	vec2(0.2, -0.15),
	vec2(-0.2, 0.1),
	vec2(0.4, -0.3)
};

// Global shader variables
vec3 N, V, F0, albedo;
float metallic, roughness, ao;
float directionalShadow;

float DistributionGGX(vec3 normal, vec3 halfway, float roughness);
float GeometrySmith(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);

vec3 calcDirectionalLight(DirectionalLight light);
float calcShadow(vec4 fragPosLightSpace, vec3 lightDir);

vec3 calculateNormal(sampler2D sampler, vec2 uv)
{
	vec3 normalValue = texture(sampler, uv).rgb; // Sample normal map texure
	normalValue = normalize(normalValue * 2.0f - 1.0f); // Transform from normalized 0.0 - 1.0 coordinates to -1.0 - 1.0
	normalValue = normalize(TBN * normalValue); // multiply by TBN to get world space normals
	return normalValue;
}

void main() 
{ 
// Flowmap calculation
    float halfCycle = blendCycle * 0.5;

    float offset = texture(flowMapNoise, Texcoord * flowNoiseSize).r * flowNoiseInfluence;

    float phase1 = mod(offset + _Time * cycleSpeed, blendCycle);
    float phase2 = mod(offset + _Time * cycleSpeed + halfCycle, blendCycle);

    vec2 flowTex = texture(flowMap, Texcoord).rg;
    vec2 flow;
    flow.x = flowTex.x * 2.0 - 1.0;
    flow.y = flowTex.y * 2.0 - 1.0;
    flow *= normalize(channelFlowDirection);

    // TODO: normalmap strength influence

    float blendFactor = abs(halfCycle - phase1) / halfCycle;
    phase1 -= halfCycle;
    phase2 -= halfCycle;

    flow *= flowSpeed * uvScale;

    vec2 layer1 = flow * phase1 + Texcoord;
    vec2 layer2 = flow * phase2 + Texcoord;

// Sampling the textures for further use
    albedo = pow(mix(texture(diffuse, layer1), texture(diffuse, layer2), blendFactor).rgb, vec3(2.2f));
	vec3 orm = mix(texture(occRouMet, layer1), texture(occRouMet, layer2), blendFactor).rgb;
    metallic = orm.b;
	roughness = orm.g;
	ao = orm.r;
	N = mix(calculateNormal(normal, layer1), calculateNormal(normal, layer2), blendFactor);

// Properties shared between lights
	V = normalize(viewPos - FragPos);

	// Reflectace calculation at normal incidence (looking perpendicularly)
	F0 = vec3(0.04f);
	F0 = mix(F0, albedo, metallic);

	vec3 Lo = vec3(0.0f);

// Directional light ------------------------------------------------------------------------
	Lo += calcDirectionalLight(directionalLight);

// Ambient light comes from convoluted cubemap irradiance
	vec3 kS = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
	vec3 kD = 1.0 - kS;
	kD *= 1.0 - metallic;
	vec3 irradiance = texture(irradianceMap, N).rgb;
	vec3 diff = irradiance * albedo;
	vec3 ambient = (kD * diff) * ao;
	vec3 color = (ambient + Lo) * (1.0 - directionalShadow * 0.95);

	// TODO: Better alpha sampling
	FragColor = vec4(color, texture(diffuse, Texcoord).a);

	// Save to second MRT
	float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0)
	{
		BrightColor = vec4(FragColor.rgb, 1.0);
	}
	else
	{
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}

vec3 calcDirectionalLight(DirectionalLight light)
{
	vec3 L = normalize(-light.direction);
	vec3 H = normalize(V + L);
	vec3 radiance = light.color.rgb * light.color.a;

	// Cook-Torrance BDRF
	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);
	vec3 F = fresnelSchlick(max(dot(H, V), 0.0f), F0);

	vec3 nominator = NDF * G * F;
	float denominator = 4 * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f) + 0.0001f; // added 0.0001f to prevent division by zero
	vec3 specular = nominator / denominator;

	// Calculate specular/albedo distribution
	vec3 kS = F; // kS equal to fresnel
	vec3 kD = vec3(1.0f) - kS; // Based on energy conservation get how much diffuse contributes to overall lightning
	kD *= 1.0f - metallic; // Multiply diffuse kD by inverse metalness because only non metals have diffuse light

	// Scale light by NdotL
	float NdotL = max(dot(N, L), 0.0f);

    // Calculate shadow
    directionalShadow = calcShadow(FragPosLightSpace, L);

	//return (1.0 - directionalShadow) * ((kD * albedo / PI + specular) * radiance * NdotL);
	return ((kD * albedo / PI + specular) * radiance * NdotL);
}

float calcShadow(vec4 fragPosLightSpace, vec3 lightDir)
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
	float bias = 0.0005 * tan(acos(dot(Normal, lightDir)));
	bias = clamp(bias, 0.0, 0.01);
	//float bias = 0.0005;
    //float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(directionalShadowMap, 0);
	for(int x = -2; x <= 2; ++x)
	{
		for(int y = -2; y <= 2; ++y)
		{
			float pcfDepth = texture(directionalShadowMap, projCoords.xy + (vec2(x, y) + jitter[int(mod((x+2) + ((y+2)*5), 8))]) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}    
	}
	shadow /= 25.0;

	if(projCoords.z > 1.0)
        shadow = 0.0;

	return shadow;
}

// Normal distribution function
float DistributionGGX(vec3 normal, vec3 halfway, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(normal, halfway), 0.0f);
	float NdotH2 = NdotH * NdotH;

	float nom = a2;
	float denom = (NdotH2 *(a2 - 1.0f) + 1.0f);
	denom = PI * denom * denom;

	return nom / denom;
}

// Geometry functions
float GeometrySmith(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness)
{
	float NdotV = max(dot(normal, viewDir), 0.0f);
	float NdotL = max(dot(normal, lightDir), 0.0f);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = roughness + 1.0f;
	float k = (r * r) / 8.0f;

	float nom = NdotV;
	float denom = NdotV * (1.0f - k) + k;

	return nom / denom;
}

// Fresnel equation
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	cosTheta = min(cosTheta, 1.0f);
	return F0 + (1.0f - F0) * pow(1.0f - cosTheta, 5.0f);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}