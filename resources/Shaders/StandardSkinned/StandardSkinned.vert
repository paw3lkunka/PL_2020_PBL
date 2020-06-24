#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec2 texcoord;
layout (location = 4) in ivec4 boneIDs;
layout (location = 5) in vec4 weights;

layout (std140, binding = 0) uniform Camera
{
    mat4 projection;
    mat4 view;
    vec3 viewPos;
};

const int MAX_BONES = 128;
layout (std140, binding = 1) uniform gBones
{
    mat4 finalTransform[MAX_BONES];
};

layout (std140, binding = 4) uniform ShadowMapping
{
	mat4 directionalLightMatrix;
};

out vec3 FragPos;
out vec3 Normal;
out vec2 Texcoord;
out vec4 FragPosLightSpace;
out mat3 TBN;

uniform mat4 model;
uniform mat4 MVP;

mat3 calculateTBN(vec3 normal, vec3 tangent)
{
	vec3 T = normalize(vec3(model * vec4(tangent, 0.0)));
	vec3 N = normalize(vec3(model * vec4(normal, 0.0)));
	vec3 bitangent = cross(N, T);
	vec3 B = normalize(vec3(model * vec4(bitangent, 0.0)));
	return mat3(T, B, N);
}

void main() 
{
	mat4 boneTransform = finalTransform[boneIDs[0]] * weights[0];
    boneTransform += finalTransform[boneIDs[1]] * weights[1];
    boneTransform += finalTransform[boneIDs[2]] * weights[2];
    boneTransform += finalTransform[boneIDs[3]] * weights[3];

	vec4 posLocal = boneTransform * vec4(position, 1.0);
	vec3 normalLocal = vec3(boneTransform * vec4(normal, 0.0));
	vec3 tangentLocal = vec3(boneTransform * vec4(tangent, 0.0));

	Texcoord = texcoord;
	
	FragPos = vec3(model * posLocal);
	Normal = mat3(transpose(inverse(model))) * normalLocal;
	TBN = calculateTBN(normalLocal, tangentLocal);
    FragPosLightSpace = directionalLightMatrix * vec4(FragPos, 1.0);
	gl_Position = MVP * posLocal;
}