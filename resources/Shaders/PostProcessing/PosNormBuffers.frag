#version 430 core

layout (location = 0) out vec3 gViewSpacePos;
layout (location = 1) out vec3 gViewSpaceNormal;

in vec2 Texcoord;
in vec3 FragPosView;
in vec3 NormalView;

uniform sampler2D diffuse;

void main()
{
    if (texture(diffuse, Texcoord).a < 0.3)
	{
		discard;
	}

    gViewSpacePos = FragPosView;
    gViewSpaceNormal = normalize(NormalView);
}

