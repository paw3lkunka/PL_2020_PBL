#version 430 core

noperspective in vec2 uv;

out vec4 frag_color;

uniform sampler2D sprite;
uniform vec4 color;

void main()
{
    frag_color = texture(sprite, uv) * color;
}