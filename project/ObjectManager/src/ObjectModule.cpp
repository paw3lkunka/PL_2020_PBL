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

void ObjectModule::readScene(std::string path)
{
    sceneReader.readScene(path);
}

#pragma region SceneWriter Wrapper
void ObjectModule::saveScene(const char* filePath)
{
    sceneWriter.saveScene(filePath);
}
#pragma endregion

#pragma region objectContainer Wrapper

MeshCustom* ObjectModule::getMeshCustomFromPath(const char* meshPath)
{
    return objectContainer.getMeshCustomFromPath(meshPath);
}

MeshSkinned* ObjectModule::getMeshSkinnedFromPath(const char* meshPath)
{
    return objectContainer.getMeshSkinnedFromPath(meshPath);
}

Material* ObjectModule::getMaterialFromName(const char* name)
{
    return objectContainer.getMaterialFromName(name);
}

#pragma endregion

#pragma region objectMaker Wrapper

Entity* ObjectModule::newEntity(int bufferSize)
{
    return objectMaker.newEntity(bufferSize);
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

void ObjectModule::newModel(const char* filePath, FileType type)
{
    for(auto m : objectContainer.meshes)
    {
        if(m->filePath == filePath)
        {
            return;
        }
    }
    objectMaker.newModel(filePath, type);
}

Material* ObjectModule::newMaterial(Shader* shader, std::string name)
{
    for(auto m : objectContainer.materials)
    {
        if(m->getName() == name)
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