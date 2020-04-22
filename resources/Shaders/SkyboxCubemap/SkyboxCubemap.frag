#version 430 core

out vec4 FragColor;

in vec3 uvw; // Direction vector of 3D texture coordinate

uniform samplerCube cubemap; // Cubemap texture sampler

void main()
{
    FragColor = texture(cubemap, uvw);
}