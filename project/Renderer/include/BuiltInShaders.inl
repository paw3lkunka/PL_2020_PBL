#ifndef _BUILTINSHADERS_INL
#define _BUILTINSHADERS_INL

namespace BuiltInShaders
{
// ? +++++ Simple vertex shader as base for error and debug shaders +++++
const char* baseVertexCode = 
R"shader(
#version 430 core

layout(location = 0) in vec3 position;

uniform mat4 MVP;

void main() 
{
    gl_Position = MVP * vec4(position, 1.0);
}
)shader";

// ? +++++ Simple pink shader used when object's material is invalid or missing +++++
const char* internalErrorFragmentCode = 
R"shader(
#version 430 core

out vec4 FragColor;

void main() 
{
    FragColor = vec4(1.0, 0.0, 1.0, 1.0);
}
)shader";

// ? +++++ Vertex shader for use with simple skybox +++++
const char* simpleCubemapVertex =
R"shader(
#version 430 core
layout (location = 0) in vec3 position;

uniform mat4 projection;
uniform mat4 view;

out vec3 localPos;

void main()
{
    localPos = position;

    mat4 rotView = mat4(mat3(view)); // Removes translation from view matrix
    vec4 clipPos = projection * rotView * vec4(localPos, 1.0);

    gl_Position = clipPos.xyww;
}
)shader";

// ? +++++ Cubemap convolution shader +++++
const char* cubemapConvolution = 
R"shader(
#version 430 core

out vec4 FragColor;
in vec3 localPos;

uniform samplerCube environmentMap;

const float PI = 3.14159265359;

void main()
{
    vec3 normal = normalize(localPos);

    vec3 irradiance = vec3(0.0);

    // Discrete sampling of the hemisphere given the integral's spherical coordinates
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = cross(up, normal);
    up = cross(normal, right);

    float sampleDelta = 0.025;
    float nrSamples = 0.0;
    for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
        for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
            // Spherical to cartesian
            vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            // Tangent space to world
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

            irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }

    irradiance = PI * irradiance * (1.0 / float(nrSamples));

    FragColor = vec4(irradiance, 1.0);
}
)shader";

}

#endif // _BUILTINSHADERS_INL
