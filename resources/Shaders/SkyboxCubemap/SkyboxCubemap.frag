#version 430 core

out vec4 FragColor;

in vec3 uvw; // Direction vector of 3D texture coordinate

uniform samplerCube cubemap; // Cubemap texture sampler

void main()
{
    vec3 envColor = texture(cubemap, uvw).rgb;

    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0 / 2.2));

    FragColor = vec4(envColor, 1.0);
}