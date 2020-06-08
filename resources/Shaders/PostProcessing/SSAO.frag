#version 430 core

out float FragColor;

in vec2 uv;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[64];
uniform mat4 projection;

uniform vec3 noiseScale;
uniform float bias;
uniform float radius;

void main()
{
    vec3 fragPos = texture(gPosition, uv).xyz;
    vec3 normal = texture(gNormal, uv).rgb;
    vec3 randomVec =  texture(texNoise, uv * vec2(noiseScale)).xyz;

    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for (int i = 0; i < 64; ++i)
    {
        // Get sample position
        vec3 samp = TBN * samples[i]; // from tangent to view space
        samp = fragPos + samp * radius;

        vec4 offset = vec4(samp, 1.0);
        offset = projection * offset; // from view-space to clip-space
        offset.xyz /= offset.w; // perspective division
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0

        float sampleDepth = texture(gPosition, offset.xy).z;
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));

        occlusion += (sampleDepth >= samp.z + bias ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = 1.0 - (occlusion / 64);

    FragColor = occlusion;
}