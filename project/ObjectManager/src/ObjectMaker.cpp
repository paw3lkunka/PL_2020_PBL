#include "ObjectMaker.hpp"
#include "ObjectModule.hpp"
#include "ObjectContainer.hpp"
#include "ObjectExceptions.inl"
#include "FileStructures.inl"

#include "Shader.hpp"
#include "Entity.hpp"
#include "Texture.hpp"
#include "Cubemap.hpp"
#include "Material.hpp"
#include "Font.hpp"
#include "Components.inc"
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

Entity* ObjectMaker::newEntity(int bufferSize, std::string name)
{
    objContainer->entities.push_back(Entity(nextID++, bufferSize, name) );
    return &objContainer->entities[objContainer->entities.size() - 1];
}

Shader* ObjectMaker::newShader(std::string shaderName, const char* vertexShaderPath, const char* fragmentShaderPath, const char* geometryShaderPath)
{
    bool loaded = true;
    loaded &= objModPtr->assetReader.loadShader(fragmentShaderPath);
    loaded &= objModPtr->assetReader.loadShader(vertexShaderPath);
    if(geometryShaderPath != nullptr)
    {
        loaded &= objModPtr->assetReader.loadShader(geometryShaderPath);
    }
    if(loaded)
    {
        std::string fragmentShaderData = objModPtr->assetReader.shaders[fragmentShaderPath];
        std::string vertexShaderData = objModPtr->assetReader.shaders[vertexShaderPath];
        if(geometryShaderPath != nullptr)
        {
            std::string geometryShaderData = objModPtr->assetReader.shaders[geometryShaderPath];
            objContainer->shaders.push_back(new Shader(shaderName, vertexShaderData.c_str(), fragmentShaderData.c_str(), geometryShaderData.c_str()));
            Shader* shaderRef = objContainer->shaders[objContainer->shaders.size() - 1];
            shaderRef->vertexShaderPath = vertexShaderPath;
            shaderRef->fragmentShaderPath = fragmentShaderPath;
            shaderRef->geometryShaderPath = geometryShaderPath;

            objModPtr->assetReader.shaders.erase( objModPtr->assetReader.shaders.find(fragmentShaderPath) );
            objModPtr->assetReader.shaders.erase( objModPtr->assetReader.shaders.find(vertexShaderPath) );
            objModPtr->assetReader.shaders.erase( objModPtr->assetReader.shaders.find(geometryShaderPath) );
            return shaderRef;
        }
        else
        {
            objContainer->shaders.push_back(new Shader(shaderName, vertexShaderData.c_str(), fragmentShaderData.c_str()));
            Shader* shaderRef = objContainer->shaders[objContainer->shaders.size() - 1];
            shaderRef->vertexShaderPath = vertexShaderPath;
            shaderRef->fragmentShaderPath = fragmentShaderPath;

            objModPtr->assetReader.shaders.erase( objModPtr->assetReader.shaders.find(fragmentShaderPath) );
            objModPtr->assetReader.shaders.erase( objModPtr->assetReader.shaders.find(vertexShaderPath) );
            return shaderRef;
        }
    }
    throw AssetLoadingException("Shader");
}

Texture* ObjectMaker::newTexture(const char* filePath, TextureCreateInfo createInfo)
{
    if(objModPtr->assetReader.loadTexture(filePath))
    {
        auto iter = objModPtr->assetReader.textures.find(filePath);
        TextureData* texData = &iter->second;
        createInfo.width = texData->width;
        createInfo.height = texData->height;
        createInfo.format = texData->nrComponents == 1 ? GL_RED : texData->nrComponents == 3 ? GL_RGB : GL_RGBA;
        objContainer->textures.push_back(new Texture(texData->data, createInfo, filePath));
        return objContainer->textures[objContainer->textures.size() - 1];
    }
    throw AssetLoadingException("Texture");
}

Cubemap* ObjectMaker::newCubemap(TextureCreateInfo createInfo, const char* frontPath, const char* leftPath, 
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
        

        objContainer->cubemaps.push_back(new Cubemap(createInfo, frontData.data, leftData.data, rightData.data, backData.data, topData.data, bottomData.data));
        auto map = objContainer->cubemaps[objContainer->cubemaps.size() - 1];
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

void ObjectMaker::newModel(const char* filePath)
{
    objModPtr->assetReader.loadMesh(filePath);
}

Material* ObjectMaker::newMaterial(Shader* shader, std::string name, RenderType renderingType, bool instancingEnabled)
{
    objContainer->materials.push_back(new Material(shader, name.c_str(), renderingType, instancingEnabled));
    return objContainer->materials[objContainer->materials.size() - 1];
}

Animation* ObjectMaker::newAnimation(Animation& animation, std::string path, std::string name)
{
    objContainer->animations[path + "/" + name] = animation;
    return &objContainer->animations[path + "/" + name];
}

Font* ObjectMaker::newFont(const char* filePath, unsigned int size, std::string name)
{
    FT_Library ftLib;
    if (FT_Init_FreeType(&ftLib))
    {
        throw AssetLoadingException("Font: freetype library failed to initialize");
    }
    FT_Face face;
    if (FT_New_Face(ftLib, filePath, 0, &face))
    {
        throw AssetLoadingException("Font: freetype font face not created");
    }
    FT_Set_Pixel_Sizes(face, 0, size);

    objContainer->fonts.push_back(new Font(face, std::string(filePath), name, size));

    FT_Done_Face(face);
    FT_Done_FreeType(ftLib);

    return objContainer->fonts[objContainer->fonts.size() - 1];
}