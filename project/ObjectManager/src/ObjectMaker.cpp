#include "ObjectMaker.hpp"
#include "ObjectModule.hpp"
#include "ObjectContainer.hpp"
#include "ObjectExceptions.inl"

#include "Shader.hpp"
#include "Entity.hpp"

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