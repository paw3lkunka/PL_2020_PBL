#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec2 texcoord;
layout(location = 4) in ivec4 boneIDs;
layout(location = 5) in vec4 weights;

layout (std140, binding = 0) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

const int MAX_BONES = 128;
layout (std140, binding = 1) uniform gBones
{
    mat4 finalTransform[MAX_BONES];
};

out vec2 uv0;
out vec3 normal0;
out vec3 worldPos0;

uniform mat4 model;

void main()
{
    mat4 boneTransform = finalTransform[boneIDs[0]] * weights[0];
    boneTransform += finalTransform[boneIDs[1]] * weights[1];
    boneTransform += finalTransform[boneIDs[2]] * weights[2];
    boneTransform += finalTransform[boneIDs[3]] * weights[3];

    vec4 posLocal = boneTransform * vec4(position, 1.0);
    gl_Position = projection * view * model * posLocal;

    uv0 = texcoord;
    vec4 normalLocal = boneTransform * vec4(normal, 0.0);
    normal0 = (model * normalLocal).xyz;
    worldPos0 = (model * posLocal).xyz;
}
