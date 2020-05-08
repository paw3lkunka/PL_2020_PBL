#include "ObjectModule.hpp"

#include "Entity.hpp"
#include "Shader.hpp"
#include "Entity.hpp"
#include "Texture.hpp"
#include "Cubemap.hpp"
#include "Material.hpp"
#include "AssetStructers.inl"
#include "Message.inl"
#include "Core.hpp"

#include <assimp/scene.h>
#include <assimp/anim.h>

std::vector<Entity>* ObjectModule::getEntitiesVector()
{
    return &objectContainer.entities;
}

void ObjectModule::receiveMessage(Message msg)
{
    switch(msg.getEvent())
    {
        case Event::QUERY_AUDIO_DATA:
        {
            std::unordered_map<std::string, AudioFile>::iterator iter = assetReader.audioClips.find(msg.getValue<const char*>());

            if(iter != assetReader.audioClips.end())
            {
                GetCore().getMessageBus().sendMessage( Message( Event::RECEIVE_AUDIO_DATA, &iter->second ) );
            }
        }
        break;
    }
}

bool ObjectModule::compareStrings(const char* str1, const char* str2)
{
    return compareStrings(std::string(str1), std::string(str2));
}

bool ObjectModule::compareStrings(std::string str1, std::string str2)
{
    if(str1.length() != str2.length())
    {
        return false;
    }
    for(int i = 0; i < str1.length(); ++i)
    {
        if( str1[i] != str2[i])
        {
            return false;
        }
    }
    return true;
}

#pragma region Scene Wrapper
void ObjectModule::saveScene(const char* filePath)
{
    sceneWriter.saveScene(filePath);
}

void ObjectModule::readScene(std::string path)
{
    sceneReader.readScene(path);
}

#pragma endregion


#pragma region objectMaker Wrapper

Entity* ObjectModule::newEntity(int bufferSize, std::string name)
{
    for(int i = 0; i < objectContainer.entities.size(); ++i)
    {
        if(objectContainer.entities[i].getName() != "" && objectContainer.entities[i].getName() == name)
        {
            return &objectContainer.entities[i];
        }
    }
    return objectMaker.newEntity(bufferSize, name);
}

Shader* ObjectModule::newShader(const char* vertexShaderPath, const char* fragmentShaderPath, const char* geometryShaderPath)
{   
    for(Shader* s : objectContainer.shaders)
    {
        if(s->vertexShaderPath == std::string(vertexShaderPath) 
        && s->fragmentShaderPath == std::string(fragmentShaderPath) )
        {
            return s;
        }
    }
    return objectMaker.newShader(vertexShaderPath, fragmentShaderPath, geometryShaderPath);
}

Texture* ObjectModule::newTexture(const char* filePath, TextureCreateInfo createInfo)
{
    for(Texture* t : objectContainer.textures)
    {
        if(t->filePath == filePath)
        {
            return t;
        }
    }
    return objectMaker.newTexture(filePath, createInfo);
}

Cubemap* ObjectModule::newCubemap(TextureCreateInfo createInfo, const char* frontPath, const char* leftPath, 
                    const char* rightPath, const char* backPath, const char* topPath, const char* bottomPath)
{
    for(Cubemap* c : objectContainer.cubemaps)
    {
        if(c->frontPath == frontPath && c->backPath == backPath 
        && c->leftPath == leftPath && c->rightPath == rightPath 
        && c->bottomPath == bottomPath && c->topPath == topPath)
        {
            return c;
        }
    }
    return objectMaker.newCubemap(createInfo, frontPath, leftPath, rightPath, backPath, topPath, bottomPath);
}

void ObjectModule::newModel(const char* filePath)
{
    for(auto m : objectContainer.meshes)
    {
        if(m->filePath == filePath)
        {
            return;
        }
    }
    objectMaker.newModel(filePath);
}

Material* ObjectModule::newMaterial(Shader* shader, std::string name)
{
    for(auto m : objectContainer.materials)
    {
        if(compareStrings(m->getName(), name.c_str()))
        {
            return m;
        }
    }
    return objectMaker.newMaterial(shader, name);
}

void ObjectModule::newAudioClip(const char* filePath)
{
    assetReader.loadAudioClip(filePath);
}

#pragma endregion

#pragma region ObjectContainer wrapper

Bone* ObjectModule::getBonePtrByName(const char* name)
{
    try
    {
        return objectContainer.getBonePtrByName(name);
    }
    catch(const std::out_of_range& e)
    {
        return nullptr;
    }
}

Animation* ObjectModule::getAnimationPtrByName(const char* name)
{
    try
    {
        return objectContainer.getAnimationPtrByName(name);
    }
    catch(const std::out_of_range& e)
    {
        return nullptr;
    }
}

#pragma endregion