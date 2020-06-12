#version 430 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec3 uvw; // Direction vector of 3D texture coordinate

uniform samplerCube cubemap; // Cubemap texture sampler

void main()
{
    vec3 envColor = texture(cubemap, uvw).rgb;

    FragColor = vec4(envColor, 1.0);

    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0)
	{
		BrightColor = vec4(FragColor.rgb, 1.0);
	}
	else
	{
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}