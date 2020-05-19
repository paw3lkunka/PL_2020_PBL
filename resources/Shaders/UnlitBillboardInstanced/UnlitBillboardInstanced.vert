#version 430 core

layout (location = 0) in vec3 position;
layout (location = 3) in vec2 texcoord;
layout (location = 5) in mat4 instanceModel; // Assigns up to index 5

layout (std140, binding = 0) uniform Camera
{
    mat4 projection;
    mat4 view;
    vec3 viewPos;
};

out vec2 uv;

void main()
{
    gl_Position = projection * view * instanceModel * vec4(position, 1.0);
    uv = texcoord;
}