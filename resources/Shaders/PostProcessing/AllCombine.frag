#version 430 core

out vec4 FragColor;

in vec2 uv;

uniform sampler2D scene;
uniform sampler2D bloom0;
uniform sampler2D bloom1;
uniform sampler2D bloom2;
uniform sampler2D bloom3;
uniform sampler2D bloom4;
uniform sampler2D ssao;

uniform float exposure;
uniform float vignette;

void main()
{
    const float gamma = 2.2;

    vec3 hdrColor = texture(scene, uv).rgb * texture(ssao, uv).r;
    vec3 bloomColor = texture(bloom0, uv).rgb * 0.25 + texture(bloom1, uv).rgb * 0.05 + texture(bloom2, uv).rgb * 0.02 + texture(bloom3, uv).rgb * 0.01 + texture(bloom4, uv).rgb * 0.005;

    vec2 uv = uv * (1.0 - uv.yx);
    float vig = uv.x * uv.y * 15.0;
    vig = pow(vig, vignette);

    hdrColor += bloomColor; // Additive blending
    hdrColor *= vig;

    vec3 result = vec3(1.0) - exp(-hdrColor * exposure); // Tonemapping
    result = pow(result, vec3(1.0 / gamma)); // Gamma correction

    FragColor = vec4(result, 1.0);
}