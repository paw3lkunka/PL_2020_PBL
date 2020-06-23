#include "ObjectModule.hpp"

#include "Entity.hpp"
#include "Shader.hpp"
#include "Entity.hpp"
#include "Texture.hpp"
#include "Cubemap.hpp"
#include "CubemapHdr.hpp"
#include "Material.hpp"
#include "Font.hpp"
#include "AssetStructers.inl"
#include "Message.inl"
#include "Core.hpp"

#include "CameraSystem.hpp"
#include "HideoutSystem.hpp"

#include <assimp/scene.h>
#include <assimp/anim.h>

// !TEMP SHIT
#include "ScenesPaths.hpp"
#include "TerrainUtils.hpp"
#include "Components.inc"
#include "CubemapHdr.hpp"

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
            newAudioClip( msg.getValue<const char*>() );
        }
        break;
    }
}


void ObjectModule::cleanup()
{
    objectContainer.cleanup();
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

void ObjectModule::unloadSceneAndLoadNew(std::string newScenePath)
{
    objectContainer.unloadScene();

    // * setting serialization id for 1 (0 is scene root node)
    ISerializable::nextID = 1;
    ObjectMaker::nextID = 0;

    if(newScenePath.compare(Scenes::gameScene) == 0) //HACK !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    {
        ObjectModule& objectModule = GetCore().objectModule;
        RendererModule& rendererModule = GetCore().rendererModule;
        SceneModule& sceneModule = GetCore().sceneModule;
        UiModule& uiModule = GetCore().uiModule;
        #include "../../resources/Scenes/newScene.icpp"
    }
    else
    {
        //* reading scene
        sceneReader.readScene(newScenePath);
    }

    GetCore().gameSystemsModule.entities = getEntitiesVector();
}
#pragma endregion


#pragma region objectMaker Wrapper

Entity* ObjectModule::newEntity(int bufferSize, std::string name)
{
    for(int i = 0; i < objectContainer.entities.size(); ++i)
    {
        if(objectContainer.entities[i].getName() != "" && std::strcmp(objectContainer.entities[i].getName().c_str(), name.c_str()) == 0)
        {
            return &objectContainer.entities[i];
        }
    }
    return objectMaker.newEntity(bufferSize, name);
}

Shader* ObjectModule::newShader(std::string shaderName, const char* vertexShaderPath, const char* fragmentShaderPath, const char* geometryShaderPath, bool serialize)
{
    if (serialize)
    {
        for(Shader* s : objectContainer.shaders)
        {
            if(shaderName.compare(s->shaderName) == 0)
            {
                return s;
            }
        }
    }
    return objectMaker.newShader(shaderName, vertexShaderPath, fragmentShaderPath, geometryShaderPath, serialize);
}

Texture* ObjectModule::newTexture(const char* filePath, TextureCreateInfo createInfo)
{
    for(Texture* t : objectContainer.textures)
    {
        if(t->filePath.compare(filePath) == 0)
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
        if((c->frontPath.compare(frontPath) == 0) 
            && (c->backPath.compare(backPath) == 0)
            && (c->leftPath.compare(leftPath) == 0) 
            && (c->rightPath.compare(rightPath) == 0) 
            && (c->bottomPath.compare(bottomPath) == 0) 
            && (c->topPath.compare(topPath) == 0))
        {
            return c;
        }
    }
    return objectMaker.newCubemap(createInfo, frontPath, leftPath, rightPath, backPath, topPath, bottomPath);
}

CubemapHdr* ObjectModule::newHdrCubemap(TextureCreateInfo createInfo, const char* frontPath, const char* leftPath, 
                    const char* rightPath, const char* backPath, const char* topPath, const char* bottomPath)
{
    for(CubemapHdr* c : objectContainer.hdrCubemaps)
    {
        if((c->frontPath.compare(frontPath) == 0) 
            && (c->backPath.compare(backPath) == 0)
            && (c->leftPath.compare(leftPath) == 0) 
            && (c->rightPath.compare(rightPath) == 0) 
            && (c->bottomPath.compare(bottomPath) == 0) 
            && (c->topPath.compare(topPath) == 0))
        {
            return c;
        }
    }
    return objectMaker.newHdrCubemap(createInfo, frontPath, leftPath, rightPath, backPath, topPath, bottomPath);
}

void ObjectModule::newModel(const char* filePath, bool customName, bool createEntities)
{
    for(auto m : objectContainer.meshes)
    {
        if(m->filePath.compare(filePath) == 0)
        {
            return;
        }
    }
    objectMaker.newModel(filePath, customName, createEntities);
}

Material* ObjectModule::newMaterial(Shader* shader, std::string name, RenderType renderingType, bool instancingEnabled)
{
    for(auto m : objectContainer.materials)
    {
        if(strcmp(m->getName(), name.c_str()) == 0)
        {
            return m;
        }
    }
    return objectMaker.newMaterial(shader, name, renderingType, instancingEnabled);
}

void ObjectModule::newAudioClip(const char* filePath)
{
    AudioFile* data = assetReader.loadAudioClip(filePath);
    
    if(data == nullptr)
    {
        std::cout << "WARNING: Failed to load audio file: " << filePath << std::endl;
        return;
    }

    GetCore().getMessageBus().sendMessage( Message( Event::RECEIVE_AUDIO_DATA, data ) );
}

Font* ObjectModule::newFont(const char* filePath, unsigned int size, std::string name)
{
    for(auto f : objectContainer.fonts)
    {
        if(strcmp(f->getFontPath(), filePath) == 0)
        {
            return f;
        }
    }
    return objectMaker.newFont(filePath, size, name);
}

#pragma endregion

#pragma region ObjectContainer wrapper


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

Font* ObjectModule::getFontPtrByName(const char* name)
{
    return objectContainer.getFontPtrByName(name);
}

#pragma endregion