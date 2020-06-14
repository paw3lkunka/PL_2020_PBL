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

out vec3 FragPosView;
out vec3 NormalView;
out vec2 Texcoord;

uniform mat4 model;
uniform mat4 MVP;

void main()
{
    vec4 viewPosition = view * model * vec4(position, 1.0);
    FragPosView = viewPosition.xyz;
    mat3 normalMat = transpose(inverse(mat3(view * model)));
    NormalView = normalMat * normal;
    Texcoord = texcoord;

    gl_Position = projection * viewPosition;
}