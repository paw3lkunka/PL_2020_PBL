#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 3) in vec2 texcoord;

layout (std140, binding = 0) uniform Camera
{
    mat4 projection;
    mat4 view;
    vec3 viewPos;
};

layout (std140, binding = 4) uniform ShadowMapping
{
	mat4 directionalLightMatrix;
};

out vec3 FragPos;
out vec3 Normal;
out vec2 Texcoord;
out vec4 FragPosLightSpace;

uniform mat4 model;
uniform mat4 MVP;

void main() 
{
	FragPos = vec3(model * vec4(position, 1.0));
	Normal = mat3(transpose(inverse(model))) * normal;
	Texcoord = texcoord;
	FragPosLightSpace = directionalLightMatrix * vec4(FragPos, 1.0);

	gl_Position = MVP * vec4(position, 1.0);
}