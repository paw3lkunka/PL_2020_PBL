#version 430 core

layout (location = 0) in vec4 vertex; // vec2 position, vec2 texcoord
layout (location = 1) in mat3 instanceModel; // assigns up to index 3

noperspective out vec2 uv;

uniform mat4 projection;
uniform mat4 model;

void main()
{
    gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
    uv = vec2(vertex.z, 1.0 - vertex.w);
}