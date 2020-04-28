#include "ObjectMaker.hpp"
#include "ObjectModule.hpp"
#include "ObjectContainer.hpp"
#include "ObjectExceptions.inl"

#include "Shader.hpp"
#include "Entity.hpp"
#include "Texture.hpp"
#include "Cubemap.hpp"
#include "Material.hpp"
#include "AssetStructers.inl"

int ObjectMaker::nextID = 0;
bool ObjectMaker::hasInstance = false;

ObjectMaker::ObjectMaker(ObjectModule* objectmodulePtr)
{
    if(hasInstance)
    {
        throw TooManyInstancesException("ObjectMaker");
    }
    hasInstance = true;
    objModPtr = objectmodulePtr;
    objContainer = &objectmodulePtr->objectContainer;
}

Entity& ObjectMaker::newEntity(int bufferSize = 0)
{
    objContainer->entities.push_back( Entity(nextID++, bufferSize) );
    return objContainer->entities[objContainer->entities.size() - 1];
}

Shader& ObjectMaker::newShader(const char* vertexShaderPath, const char* fragmentShaderPath, const char* geometryShaderPath)
{
    bool loaded = true;
    loaded &= objModPtr->assetReader.loadShader(fragmentShaderPath);
    loaded &= objModPtr->assetReader.loadShader(vertexShaderPath);
    if(geometryShaderPath != nullptr)
    {
        loaded &= objModPtr->assetReader.loadShader(fragmentShaderPath);
    }
    if(loaded)
    {
        std::string fragmentShaderData = objModPtr->assetReader.shaders[fragmentShaderPath];
        std::string vertexShaderData = objModPtr->assetReader.shaders[vertexShaderPath];
        if(geometryShaderPath != nullptr)
        {
            std::string geometryShaderData = objModPtr->assetReader.shaders[geometryShaderPath];
            objContainer->shaders.push_back(Shader(vertexShaderData.c_str(), fragmentShaderData.c_str(), geometryShaderData.c_str()));
            Shader& shaderRef = objContainer->shaders[objContainer->shaders.size() - 1];
            shaderRef.vertexShaderPath = vertexShaderPath;
            shaderRef.fragmentShaderPath = fragmentShaderPath;
            shaderRef.geometryShaderPath = geometryShaderPath;
            return shaderRef;
        }
        else
        {
            objContainer->shaders.push_back(Shader(vertexShaderData.c_str(), fragmentShaderData.c_str()));
            Shader& shaderRef = objContainer->shaders[objContainer->shaders.size() - 1];
            shaderRef.vertexShaderPath = vertexShaderPath;
            shaderRef.fragmentShaderPath = fragmentShaderPath;
            return shaderRef;
        }
    }
    throw AssetLoadingException("Shader");
}

Texture& ObjectMaker::newTexture(const char* filePath, TextureCreateInfo createInfo)
{
    if(objModPtr->assetReader.loadTexture(filePath))
    {
        TextureData texData = objModPtr->assetReader.textures[filePath];
        createInfo.width = texData.width;
        createInfo.height = texData.height;
        createInfo.format = texData.nrComponents == 1 ? GL_RED : texData.nrComponents == 3 ? GL_RGB : GL_RGBA;

        objContainer->textures.push_back(Texture(texData.data, createInfo, filePath));
        return objContainer->textures[objContainer->textures.size() - 1];
    }
    throw AssetLoadingException("Texture");
}

Cubemap& ObjectMaker::newCubemap(TextureCreateInfo createInfo, const char* frontPath, const char* leftPath, 
                    const char* rightPath, const char* backPath, const char* topPath, const char* bottomPath)
{
    bool loaded = true;
    loaded &= objModPtr->assetReader.loadTexture(frontPath);
    loaded &= objModPtr->assetReader.loadTexture(leftPath);
    loaded &= objModPtr->assetReader.loadTexture(rightPath);
    loaded &= objModPtr->assetReader.loadTexture(backPath);
    loaded &= objModPtr->assetReader.loadTexture(topPath);
    loaded &= objModPtr->assetReader.loadTexture(bottomPath);

    if(loaded)
    {
        TextureData frontData = objModPtr->assetReader.textures[frontPath];
        createInfo.width = frontData.width;
        createInfo.height = frontData.height;
        createInfo.format = frontData.nrComponents == 1 ? GL_RED : frontData.nrComponents == 3 ? GL_RGB : GL_RGBA;
        TextureData leftData = objModPtr->assetReader.textures[leftPath];
        TextureData rightData = objModPtr->assetReader.textures[rightPath];
        TextureData backData = objModPtr->assetReader.textures[backPath];
        TextureData topData = objModPtr->assetReader.textures[topPath];
        TextureData bottomData = objModPtr->assetReader.textures[bottomPath];
        

        objContainer->cubemaps.push_back(Cubemap(createInfo, frontData.data, leftData.data, rightData.data, backData.data, topData.data, bottomData.data));
        auto map = &objContainer->cubemaps[objContainer->cubemaps.size() - 1];
        map->frontPath = frontPath;
        map->bottomPath = bottomPath;
        map->leftPath = leftPath;
        map->rightPath = rightPath;
        map->topPath = topPath;
        map->backPath = backPath;
        return objContainer->cubemaps[objContainer->cubemaps.size() - 1];
    }
    throw AssetLoadingException("Cubemap");
}