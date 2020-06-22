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

const float FXAA_MAX_TEXEL_SIZE = 8.0;
const float FXAA_MIN_REDUCE = 1.0 / 128.0;
const float FXAA_BIAS_REDUCE = 1.0 / 8.0;

vec3 calcFXAA(sampler2D sceneTex);

void main()
{
    const float gamma = 2.2;

    //vec3 hdrColor = texture(scene, uv).rgb * texture(ssao, uv).r;
    vec3 hdrColor = calcFXAA(scene) * texture(ssao, uv).r;
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

vec3 calcFXAA(sampler2D sceneTex)
{
    vec2 texOffset = 1.0 / textureSize(sceneTex, 0); // Get the size of current texel
    vec3 luma = vec3(0.299, 0.587, 0.114);
    float lumaTL = dot(luma, texture2D(sceneTex, uv + vec2(-texOffset.x, -texOffset.y)).rgb);
    float lumaTR = dot(luma, texture2D(sceneTex, uv + vec2(texOffset.x, -texOffset.y)).rgb);
    float lumaBL = dot(luma, texture2D(sceneTex, uv + vec2(-texOffset.x, texOffset.y)).rgb);
    float lumaBR = dot(luma, texture2D(sceneTex, uv + vec2(texOffset.x, texOffset.y)).rgb);
    float lumaM = dot(luma, texture2D(sceneTex, uv).rgb);

    vec2 blurDirection;
    blurDirection.x = -((lumaTL + lumaTR) - (lumaBL + lumaBR));
    blurDirection.y = (lumaTL + lumaBL) - (lumaTR + lumaBR);

    float reduction = max((lumaTL + lumaTR + lumaBL + lumaBR) * (0.25 * FXAA_BIAS_REDUCE), FXAA_MIN_REDUCE);
    float scale = 1.0 / (min(abs(blurDirection.x), abs(blurDirection.y)) + reduction);

    blurDirection = clamp(vec2(FXAA_MAX_TEXEL_SIZE, FXAA_MAX_TEXEL_SIZE), vec2(-FXAA_MAX_TEXEL_SIZE, -FXAA_MAX_TEXEL_SIZE), blurDirection * scale) * texOffset;

    vec3 blur1 = 0.5 * (texture2D(sceneTex, uv + blurDirection * vec2(1.0/3.0 - 0.5)).rgb + texture2D(sceneTex, uv + blurDirection * vec2(2.0/3.0 - 0.5)).rgb);
    vec3 blur2 = 0.25 * (texture2D(sceneTex, uv + blurDirection * vec2(0.0/3.0 - 0.5)).rgb + texture2D(sceneTex, uv + blurDirection * vec2(3.0/3.0 - 0.5)).rgb);

    float lumaMin = min(lumaM, min(min(lumaTL, lumaTR), min(lumaBL, lumaBR)));
    float lumaMax = max(lumaM, max(max(lumaTL, lumaTR), max(lumaBL, lumaBR)));
    float lumaResult = dot(luma, 0.5 * blur1 + blur2);

    if (lumaResult < lumaMin || lumaResult > lumaMax)
    {
        return blur1;
    }
    else
    {
        return 0.5 * blur1 + blur2;
    }
}