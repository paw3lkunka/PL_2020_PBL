#version 430 core

noperspective in vec2 uv;

out vec4 frag_color;

uniform sampler2D sprite;
uniform vec4 color;
uniform float fadingPercentage;

void main()
{
    float alpha = 1.0;
    if((uv.x < (0.5 - fadingPercentage / 2.0)) || (uv.x > (0.5 + fadingPercentage / 2.0)))
    {
        alpha = 0.0;
    }
    frag_color = texture(sprite, uv) * vec4(color.rgb, color.a * alpha);
}