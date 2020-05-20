#version 430 core

layout (location = 0) in vec3 position;

layout (std140, binding = 0) uniform Camera
{
    mat4 projection;
    mat4 view;
    vec3 viewPos;
};

uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(position, 1.0);
}