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

// ===== Terrain channels =====
// 0
uniform vec2 uv0scale;
uniform sampler2D diffuse0;
uniform sampler2D normal0;
uniform sampler2D orm0;
// 1
uniform vec2 uv1scale;
uniform sampler2D diffuse1;
uniform sampler2D normal1;
uniform sampler2D orm1;
// 2
uniform vec2 uv2scale;
uniform sampler2D diffuse2;
uniform sampler2D normal2;
uniform sampler2D orm2;
// 3
uniform vec2 uv3scale;
uniform sampler2D diffuse3;
uniform sampler2D normal3;
uniform sampler2D orm3;

// ===== Splatmap =====
uniform sampler2D splatmap;

// ===== Shadow mapping =====
uniform sampler2D directionalShadowMap;

// ===== Environment lightning =====
uniform samplerCube irradianceMap;

// ===== Vertex shader inputs =====
in vec3 FragPos;
in vec3 Normal;
in vec2 Texcoord;
in mat3 TBN;
in vec4 FragPosLightSpace;

// ===== Shader constants =====
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

// ===== Global shader variables =====
vec3 N, V, F0, albedo;
float metallic, roughness, ao;
float directionalShadow;

// ===== Internal function prototypes =====
float DistributionGGX(vec3 normal, vec3 halfway, float roughness);
float GeometrySmith(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
vec3 calculateNormal(sampler2D sampler, vec2 uv);
vec3 unpackNormal(vec3 n);
vec3 getTriplanarBlend(vec3 norm);
vec3 getTriplanarNormal(sampler2D norm, vec2 uvscale, vec3 blend);
vec3 NormalBlend_Whiteout(vec3 n1, vec3 n2);

// ===== Lightning calculations =====
vec3 calcDirectionalLight(DirectionalLight light);
float calcShadow(vec4 fragPosLightSpace, vec3 lightDir);

void main() 
{
    // Sampling the splatmap
    vec4 splatVal = texture(splatmap, Texcoord);

	vec3 blend = getTriplanarBlend(Normal);

    // Sampling the textures by splatmap channel value
    // 0
	
	vec2 uv0s = uv0scale * 0.01;
	vec3 albedo0x = pow(texture(diffuse0, FragPos.yz * uv0s).rgb, vec3(2.2f));
	vec3 albedo0y = pow(texture(diffuse0, FragPos.xz * uv0s).rgb, vec3(2.2f));
	vec3 albedo0z = pow(texture(diffuse0, FragPos.xy * uv0s).rgb, vec3(2.2f));
    vec3 albedo0 = albedo0x * blend.x + albedo0y * blend.y + albedo0z * blend.z; 
	vec3 norm0 = getTriplanarNormal(normal0, uv0s, blend);
	vec3 occRouMet0x = texture(orm0, FragPos.yz * uv0s).rgb;
	vec3 occRouMet0y = texture(orm0, FragPos.xz * uv0s).rgb;
	vec3 occRouMet0z = texture(orm0, FragPos.xy * uv0s).rgb;
    vec3 occRouMet0 = occRouMet0x * blend.x + occRouMet0y * blend.y + occRouMet0z * blend.z;
    // 1
	vec2 uv1s = uv1scale * 0.01;
	vec3 albedo1x = pow(texture(diffuse1, FragPos.yz * uv1s).rgb, vec3(2.2f));
	vec3 albedo1y = pow(texture(diffuse1, FragPos.xz * uv1s).rgb, vec3(2.2f));
	vec3 albedo1z = pow(texture(diffuse1, FragPos.xy * uv1s).rgb, vec3(2.2f));
    vec3 albedo1 = albedo1x * blend.x + albedo1y * blend.y + albedo1z * blend.z;
	vec3 norm1 = getTriplanarNormal(normal1, uv1s, blend);
    vec3 occRouMet1x = texture(orm1, FragPos.yz * uv1s).rgb;
	vec3 occRouMet1y = texture(orm1, FragPos.xz * uv1s).rgb;
	vec3 occRouMet1z = texture(orm1, FragPos.xy * uv1s).rgb;
    vec3 occRouMet1 = occRouMet1x * blend.x + occRouMet1y * blend.y + occRouMet1z * blend.z;
    // 2
    vec2 uv2s = uv2scale * 0.01;
	vec3 albedo2x = pow(texture(diffuse2, FragPos.yz * uv2s).rgb, vec3(2.2f));
	vec3 albedo2y = pow(texture(diffuse2, FragPos.xz * uv2s).rgb, vec3(2.2f));
	vec3 albedo2z = pow(texture(diffuse2, FragPos.xy * uv2s).rgb, vec3(2.2f));
    vec3 albedo2 = albedo2x * blend.x + albedo2y * blend.y + albedo2z * blend.z; 
	vec3 norm2 = getTriplanarNormal(normal2, uv2s, blend);
	vec3 occRouMet2x = texture(orm2, FragPos.yz * uv2s).rgb;
	vec3 occRouMet2y = texture(orm2, FragPos.xz * uv2s).rgb;
	vec3 occRouMet2z = texture(orm2, FragPos.xy * uv2s).rgb;
    vec3 occRouMet2 = occRouMet2x * blend.x + occRouMet2y * blend.y + occRouMet2z * blend.z;
    // 3
    vec2 uv3s = uv3scale * 0.01;
	vec3 albedo3x = pow(texture(diffuse3, FragPos.yz * uv3s).rgb, vec3(2.2f));
	vec3 albedo3y = pow(texture(diffuse3, FragPos.xz * uv3s).rgb, vec3(2.2f));
	vec3 albedo3z = pow(texture(diffuse3, FragPos.xy * uv3s).rgb, vec3(2.2f));
    vec3 albedo3 = albedo3x * blend.x + albedo3y * blend.y + albedo3z * blend.z; 
	vec3 norm3 = getTriplanarNormal(normal3, uv3s, blend);
	vec3 occRouMet3x = texture(orm3, FragPos.yz * uv3s).rgb;
	vec3 occRouMet3y = texture(orm3, FragPos.xz * uv3s).rgb;
	vec3 occRouMet3z = texture(orm3, FragPos.xy * uv3s).rgb;
    vec3 occRouMet3 = occRouMet3x * blend.x + occRouMet3y * blend.y + occRouMet3z * blend.z;

    // Texture values get mixed based on splatmap color
    // For simplicity we assue the sum of splatmap colors never exceeds 1.0
	albedo = albedo0 * splatVal.r + albedo1 * splatVal.g + albedo2 * splatVal.b + albedo3 * splatVal.a;
	ao = occRouMet0.r * splatVal.r + occRouMet1.r * splatVal.g + occRouMet2.r * splatVal.b + occRouMet3.r * splatVal.a;
    roughness = occRouMet0.g * splatVal.r + occRouMet1.g * splatVal.g + occRouMet2.g * splatVal.b + occRouMet3.g * splatVal.a;
    metallic = occRouMet0.b * splatVal.r + occRouMet1.b * splatVal.g + occRouMet2.b * splatVal.b + occRouMet3.b * splatVal.a;

// Properties shared between lights
	//N = NormalBlend_Whiteout(NormalBlend_Whiteout(NormalBlend_Whiteout(norm0* splatVal.r, norm1* splatVal.g), norm2 * splatVal.b), norm3 * splatVal.a);
	N = norm0 * splatVal.r + norm1 * splatVal.g + norm2 * splatVal.b + norm3 * splatVal.a;
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
	vec3 color = (ambient + Lo) * (1.0 - directionalShadow * 0.7);

	FragColor = vec4(color, 1.0);

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
	float denominator = 4 * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f) + 0.0001f; // added 0.001f to prevent division by zero
	vec3 specular = nominator / denominator;

	// Calculate specular/albedo distribution
	vec3 kS = F; // kS equal to fresnel
	vec3 kD = vec3(1.0f) - kS; // Based on energy conservation get how much diffuse contributes to overall lightning
	kD *= 1.0f - metallic; // Multiply diffuse kD by inverse metalness because only non metals have diffuse light

	// Scale light by NdotL
	float NdotL = max(dot(N, L), 0.0f);

    // Calculate shadow
    directionalShadow = calcShadow(FragPosLightSpace, L);

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
	//float bias = 0.0005 * tan(acos(dot(Normal, lightDir)));
	//bias = clamp(bias, 0.0, 0.01);
	float bias = 0.0005;
    //float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(directionalShadowMap, 0);
	for(int x = -2; x <= 2; ++x)
	{
		for(int y = -2; y <= 2; ++y)
		{
			// TODO: better shadow filtering
			//float pcfDepth = texture(directionalShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			float pcfDepth = texture(directionalShadowMap, projCoords.xy + (vec2(x, y) + jitter[int(mod((x+2) + ((y+2)*5), 8))]) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}    
	}
	shadow /= 25.0;

	if(projCoords.z > 1.0)
        shadow = 0.0;

	return shadow;
}

vec3 calculateNormal(sampler2D sampler, vec2 uv)
{
	vec3 normalValue = texture(sampler, uv).rgb; // Sample normal map texure
	normalValue = normalize(normalValue * 2.0f - 1.0f); // Transform from normalized 0.0 - 1.0 coordinates to -1.0 - 1.0
	normalValue = normalize(TBN * normalValue); // multiply by TBN to get world space normals
	return normalValue;
}

vec3 unpackNormal(vec3 n)
{
	n = n * 2.0 - 1.0;
	n = -n;
	return n;
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

vec3 getTriplanarBlend(vec3 norm)
{
	vec3 blend = abs(norm);
	blend = max(blend - 0.2, 0.0);
	blend /= dot(blend, vec3(1.0, 1.0, 1.0));
	return blend;
}

vec3 getTriplanarNormal(sampler2D norm, vec2 uvscale, vec3 blend)
{
	vec3 norm0x = unpackNormal(texture(norm, FragPos.yz * uvscale).xyz);
	vec3 norm0y = unpackNormal(texture(norm, FragPos.xz * uvscale).xyz);
	vec3 norm0z = unpackNormal(texture(norm, FragPos.xy * uvscale).xyz);
	norm0x = vec3(norm0x.xy + Normal.zy, abs(norm0x.z) * Normal.x);
	norm0y = vec3(norm0y.xy + Normal.xz, abs(norm0y.z) * Normal.y);
	norm0z = vec3(norm0z.xy + Normal.xy, abs(norm0z.z) * Normal.z);
	return normalize(norm0x.zyx * blend.x + norm0y.xzy * blend.y + norm0z.xyz * blend.z);
}

vec3 NormalBlend_Whiteout(vec3 n1, vec3 n2)
{
	return normalize(vec3(n1.xy + n2.xy, n1.z * n2.z));    
}