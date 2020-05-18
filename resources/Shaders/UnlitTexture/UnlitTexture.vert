#version 430 core

layout (location = 0) in vec3 position;
layout (location = 3) in vec2 texcoord;

layout (std140, binding = 0) uniform Camera
{
    mat4 projection;
    mat4 view;
    vec3 viewPos;
};

uniform mat4 model;

out vec2 uv;

void main()
{
    uv = texcoord;
    gl_Position = projection * view * model * vec4(position, 1.0);
}