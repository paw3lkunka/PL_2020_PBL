#include "ObjectModule.hpp"

#include "Entity.hpp"
#include "Shader.hpp"
#include "Entity.hpp"
#include "Texture.hpp"
#include "Cubemap.hpp"
#include "Material.hpp"
#include "Font.hpp"
#include "AssetStructers.inl"
#include "Message.inl"
#include "Core.hpp"

#include "CameraSystem.hpp"
#include "HideoutSystem.hpp"

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
        case Event::LOAD_SCENE:
            unloadSceneAndLoadNew(msg.getValue<const char*>());
        break;
    }
}

bool ObjectModule::compareStrings(const char* str1, const char* str2)
{
    return std::strcmp(str1, str2) == 0;
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

void ObjectModule::unloadSceneAndLoadNew(std::string newScenePath)
{
    GetCore().audioModule.cleanup();
    objectContainer.unloadScene();
    // ! removing all associations for scene root node
    GetCore().sceneModule.unloadScene();
    // ! removing all root nodes from UI
    GetCore().uiModule.unloadScene();
    // ! clear message bus, for omitting messages between scenes
    GetCore().messageBus.clearBuffers();
    // * setting serialization id for 1 (0 is scene root node)
    ISerializable::nextID = 1;
    //* reading scene
    sceneReader.readScene(newScenePath);

    // ! ----- Renderer initialization block -----
    RendererModuleCreateInfo rendererCreateInfo = {};
    rendererCreateInfo.clearColor = glm::vec3(0.0f, 1.0f, 0.0f);
    rendererCreateInfo.clearFlags = GL_DEPTH_BUFFER_BIT;
    rendererCreateInfo.cullFace = true;
    rendererCreateInfo.cullFaceMode = GL_BACK;
    rendererCreateInfo.cullFrontFace = GL_CCW;
    rendererCreateInfo.depthTest = true;
    rendererCreateInfo.wireframeMode = false;
    GetCore().rendererModule.initialize(GetCore().getWindowPtr(), rendererCreateInfo, getMaterialPtrByName("skyboxMat"));
    // ! Finding main camera
    CameraSystem::setAsMain(getEntityPtrByName("Camera"));

    // ! some setup
    GetCore().gameSystemsModule.entities = getEntitiesVector();
    GetCore().sceneModule.updateTransforms();
    GetCore().uiModule.updateRectTransforms();
    GetCore().editorModule.setup();
    // ! ----- START SYSTEM FUNCTION -----
    GetCore().gameSystemsModule.run(System::START);
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

Material* ObjectModule::newMaterial(Shader* shader, std::string name, RenderType renderingType, bool instancingEnabled)
{
    for(auto m : objectContainer.materials)
    {
        if(compareStrings(m->getName(), name.c_str()))
        {
            return m;
        }
    }
    return objectMaker.newMaterial(shader, name, renderingType, instancingEnabled);
}

void ObjectModule::newAudioClip(const char* filePath)
{
    assetReader.loadAudioClip(filePath);
}

Font* ObjectModule::newFont(const char* filePath, unsigned int size, std::string name)
{
    for(auto f : objectContainer.fonts)
    {
        if(compareStrings(f->getFontPath(), filePath))
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