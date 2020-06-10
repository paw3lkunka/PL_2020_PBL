#version 430 core

out vec4 FragColor;

in vec2 uv;

uniform sampler2D scene;
uniform sampler2D bloom0;
uniform sampler2D bloom1;
uniform sampler2D bloom2;
uniform sampler2D ssao;

uniform float exposure;

void main()
{
    const float gamma = 2.2;

    vec3 hdrColor = texture(scene, uv).rgb * texture(ssao, uv).r;
    vec3 bloomColor = texture(bloom0, uv).rgb + texture(bloom1, uv).rgb * 0.5 + texture(bloom2, uv).rgb * 0.2;

    hdrColor += bloomColor; // Additive blending

    vec3 result = vec3(1.0) - exp(-hdrColor * exposure); // Tonemapping
    result = pow(result, vec3(1.0 / gamma)); // Gamma correction

    FragColor = vec4(result, 1.0);
}