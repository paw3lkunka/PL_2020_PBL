#version 430 core

layout (location = 0) in vec4 vertex;

noperspective out vec2 uv;

uniform mat4 projection;
uniform mat4 model;

void main()
{
    gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
    uv = vec2(vertex.z, vertex.w);
}