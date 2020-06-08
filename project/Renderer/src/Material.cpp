#include "Material.hpp"
#include "Core.hpp"
#include "Message.inl"

#include <iostream>

unsigned int Material::idCount = 1;

Material::Material(Shader* shader, const char* name, RenderType renderType, bool enableInstancing, bool serialize)
    : ISerializable(serialize), shader(shader), name(name), enableInstancing(enableInstancing), renderType(renderType)
{
    // Set material ID on construction
    ID = idCount;
    idCount++;

    std::unordered_map<std::string, GLenum>& uniforms = shader->getUniforms();
    for(auto var : uniforms)
    {
        switch (var.second)
        {
        case GL_SAMPLER_2D:
        case GL_SAMPLER_CUBE:
            textures.insert(std::pair<std::string, Texture*>(var.first, nullptr));
            break;
        case GL_INT:
            ints[var.first] = 0;
            break;
        case GL_FLOAT:
            floats[var.first] = 0.0f;
            break;
        case GL_FLOAT_VEC3:
            vec3s[var.first] = glm::vec3(0);
            break;
        case GL_FLOAT_VEC4:
            vec4s[var.first] = glm::vec4(0);
            break;
        case GL_FLOAT_MAT4:
            mat4s[var.first] = glm::mat4(0);
            break;
        default:
            // ! Undefined uniform type !
            // TODO: Throw exception here
            std::cerr << "Material " << name << " encountered an unsupported uniform type with name " << var.first << '\n';
            break;
        }
    }

    bool instancingSupported = false;
    std::unordered_map<std::string, GLenum>& attributes = shader->getAttributes();
    
    for(auto var : attributes)
    {
        if (var.first == "instanceModel")
        {
            instancingSupported = true;
            break;
        }
    }

    if (enableInstancing && !instancingSupported)
    {
        enableInstancing = false;
        // TODO: Proper error logging
        std::cerr << "Instancing for material " << name << " is not supported! Falling back to normal rendering.\n";
    }
}

void Material::use()
{
    if (shader != nullptr && ID != RendererModule::lastMatID)
    {
        RendererModule::lastMatID = ID;

        shader->use();

        int i = 0;
        // * ===== Texture samplers =====
        for(std::pair<std::string, Texture*> texture : textures)
        {
            if (texture.second != nullptr)
            {
                texture.second->bind(i);
                shader->setInt(texture.first, i);
                ++i;
            }
        }

        // * ===== Ints =====
        for(auto var : ints)
        {
            shader->setInt(var.first, var.second);
        }

        // * ===== Floats =====
        for(auto var : floats)
        {
            shader->setFloat(var.first, var.second);
        }
        
        // * ===== Vec3s =====
        for(auto var : vec3s)
        {
            shader->setVec3(var.first, var.second);
        }

        // * ===== Vec4s =====
        for(auto var : vec4s)
        {
            shader->setVec4(var.first, var.second);
        }

        // * ===== Mat4s =====
        for(auto var : mat4s)
        {
            shader->setMat4(var.first, var.second);
        }
    }
    else
    {
        std::cerr << "Material " << name << " has no shader!\n";
    }
}

void Material::setMVP(const glm::mat4& M, const glm::mat4& VP, std::string name)
{
    glm::mat4 model = VP * M;
    shader->setMat4(name, model);
}

void Material::setModel(const glm::mat4& M, std::string name)
{
    shader->setMat4(name, M);
}

void Material::setTexture(std::string name, Texture* value)
{
    std::unordered_map<std::string, Texture*>::iterator texturesIter = textures.find(name);
    if (texturesIter != textures.end())
    {
        texturesIter->second = value;
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

void Material::setVec3(std::string name, glm::vec3 value)
{
    std::unordered_map<std::string, glm::vec3>::iterator vec3sIter = vec3s.find(name);
    if (vec3sIter != vec3s.end())
    {
        vec3sIter->second = value;
    }
    else
    {
        // ! Name not found, aborting !
        // TODO: Insert appropriate debug log
    }
}

void Material::setVec4(std::string name, glm::vec4 value)
{
    std::unordered_map<std::string, glm::vec4>::iterator vec4sIter = vec4s.find(name);
    if (vec4sIter != vec4s.end())
    {
        vec4sIter->second = value;
    }
    else
    {
        // ! Name not found, aborting !
        // TODO: Insert appropriate debug log
    }
}

void Material::setMat4(std::string name, glm::mat4 value)
{
    std::unordered_map<std::string, glm::mat4>::iterator mat4sIter = mat4s.find(name);
    if (mat4sIter != mat4s.end())
    {
        mat4sIter->second = value;
    }
    else
    {
        // ! Name not found, aborting !
        // TODO: Insert appropriate debug log
    }
}

Shader* Material::getShaderPtr()
{
    return shader;
}

const char* Material::getName()
{
    return name.c_str();
}

int Material::getInt(std::string name)
{
    std::unordered_map<std::string, int>::iterator intsIter = ints.find(name);
    if (intsIter != ints.end())
    {
        return intsIter->second;
    }
    else
    {
        // TODO: Debug log
        return 0;
    }
}

float Material::getFloat(std::string name)
{
    auto iter = floats.find(name);
    if (iter != floats.end())
    {
        return iter->second;
    }
    else
    {
        return 0.0f;
    }
}

const glm::vec3* Material::getVec3Ptr(std::string name)
{
    auto iter = vec3s.find(name);
    if (iter != vec3s.end())
    {
        return &iter->second;
    }
    else
    {
        return nullptr;
    }
}

const glm::vec4* Material::getVec4Ptr(std::string name)
{
    auto iter = vec4s.find(name);
    if (iter != vec4s.end())
    {
        return &iter->second;
    }
    else
    {
        return nullptr;
    }
}

const glm::mat4* Material::getMat4Ptr(std::string name)
{
    auto iter = mat4s.find(name);
    if (iter != mat4s.end())
    {
        return &iter->second;
    }
    else
    {
        return nullptr;
    }
}

const Texture* Material::getTexturePtr(std::string name)
{
    auto iter = textures.find(name);
    if (iter != textures.end())
    {
        return iter->second;
    }
    else
    {
        return nullptr;
    }
}
