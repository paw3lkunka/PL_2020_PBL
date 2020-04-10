#include "Material.hpp"

#include <iostream>

int Material::idCount = 0;

Material::Material(Shader* shader) : shader(shader)
{
    // Set material ID on construction
    ID = idCount;
    idCount++;

    std::unordered_map<std::string, GLenum> uniforms = shader->getUniforms();
    for(auto var : uniforms)
    {
        std::cout << "Found uniform: " << var.first << '\n';
        switch (var.second)
        {
        case GL_INT:
            std::cout << "Type: int\n";
            ints[var.first] = 0;
            break;
        case GL_FLOAT:
            std::cout << "Type: float\n";
            floats[var.first] = 0.0f;
            break;
        case GL_FLOAT_VEC3:
            std::cout << "Type: vec3\n";
            vec3s[var.first] = glm::vec3(0);
            break;
        case GL_FLOAT_VEC4:
            std::cout << "Type: vec4\n";
            vec4s[var.first] = glm::vec4(0);
            break;
        case GL_FLOAT_MAT4:
            std::cout << "Type: mat4\n";
            mat4s[var.first] = glm::mat4(0);
            break;
        default:
            // ! Undefined uniform type !
            // TODO: Throw exception here
            break;
        }
    }
}

void Material::setInt(std::string name, int value)
{
    std::unordered_map<std::string, int>::iterator intsIter = ints.find(name);
    if (intsIter != ints.end())
    {
        intsIter->second = value;
    }
    else
    {
        // ! Name not found, aborting !
        // TODO: Insert appropriate debug log
    }
}

void Material::setFloat(std::string name, float value)
{
    std::unordered_map<std::string, float>::iterator floatsIter = floats.find(name);
    if (floatsIter != floats.end())
    {
        floatsIter->second = value;
    }
    else
    {
        // ! Name not found, aborting !
        // TODO: Insert appropriate debug log
    }
}