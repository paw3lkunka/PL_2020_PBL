#version 430 core

in vec2 uv;

layout(location = 0) out float4 _accum;
layout(location = 1) out float _revealage;

uniform sampler2D mainTex;
uniform vec4 color;

void writePixel(vec4 premultipliedReflect, vec3 transmit, float csZ)
{
    premultipliedReflect.a *= 1.0 - clamp((transmit.r + transmit.g + transmit.b) * (1.0 / 3.0), 0, 1);
    float a = min(1.0, premultipliedReflect.a) * 8.0 + 0.01;
    float b = -gl_FragCoord.z * 0.95 + 1.0;

    float w = clamp(a * a * a * 1e8 * b * b * b, 1e-2, 3e2);
    _accum = premultipliedReflect * w;
    _revealage = premultipliedReflect.a;
}

void main()
{
    writePixel(texture(mainTex, uv) * color, );
}