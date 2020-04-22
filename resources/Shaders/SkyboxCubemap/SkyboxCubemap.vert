#version 430 core

layout(location = 0) in vec3 position;

layout (std140, binding = 0) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 viewStatic;

out vec3 uvw;

void main()
{
    uvw = position;
    vec4 pos = projection * viewStatic * vec4(position, 1.0);
    gl_Position = pos.xyww;
}