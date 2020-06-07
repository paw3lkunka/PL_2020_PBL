#version 430 core

out vec4 FragColor;

in vec2 uv;

uniform sampler2D scene;
uniform sampler2D bloom;

uniform float exposure;

void main()
{
    const float gamma = 2.2;

    vec3 hdrColor = texture(scene, uv).rgb;
    vec3 bloomColor = texture(bloom, uv).rgb;

    hdrColor += bloomColor; // Additive blending

    vec3 result = vec3(1.0) - exp(-hdrColor * exposure); // Tonemapping
    result = pow(result, vec3(1.0 / gamma)); // Gamma correction

    FragColor = vec4(result, 1.0);
}