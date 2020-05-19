#version 430 core

in vec2 uv;

out vec4 frag_color;

uniform vec4 color;
uniform sampler2D mainTex;

void main()
{
    frag_color = texture(mainTex, uv) * color;
}