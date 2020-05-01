#include "SceneReader.hpp"
#include "ObjectModule.hpp"
#include "ObjectExceptions.inl"

#include "FileStructures.inl"

#include <fstream>
#include <iostream>

bool SceneReader::hasInstance = false;

SceneReader::SceneReader(ObjectModule* objModPtr) : objModulePtr(objModPtr) 
{
    if(hasInstance)
    {
        throw TooManyInstancesException("SceneReader");
    }
    hasInstance = true;
}

void SceneReader::readScene(std::string filePath)
{
    std::ifstream file(filePath);
    if(file.good())
    {
        file >> j;
    }
    file.close();
    readShaders();
    readTextures();
    readCubemaps();
    readMaterials();
    readMeshes();
    readComponents();
    readEntities();
}

void SceneReader::readShaders()
{
    int shadersAmount = j.at("Amounts").at("shaders").get<int>();
    std::string name = "";
    std::string fragmentShaderPath;
    std::string vertexShaderPath;
    std::string geometryShaderPath = "";

    Shader* shader;
    
    for(int i = 0; i < shadersAmount; ++i)
    {
        name = setName("shader", i);
        fragmentShaderPath = j.at(name).at("fragmentShaderPath").get<std::string>();
        vertexShaderPath = j.at(name).at("vertexShaderPath").get<std::string>();

        std::cout << fragmentShaderPath << std::endl;
        std::cout << vertexShaderPath << std::endl;
        
        try
        {
            geometryShaderPath = j.at(name).at("geometryShaderPath").get<std::string>();
            shader = objModulePtr->newShader(vertexShaderPath.c_str(), fragmentShaderPath.c_str(), geometryShaderPath.c_str());
        }
        catch(nlohmann::detail::out_of_range)
        {
            std::cout << "Catched exception" << std::endl;
            shader = objModulePtr->newShader(vertexShaderPath.c_str(), fragmentShaderPath.c_str());
        }
        shader->serializationID = j.at(name).at("serializationID").get<unsigned int>();
    }
}

void SceneReader::readTextures()
{
    std::string name = "";
    TextureCreateInfo texCreateInfo;
    int texturesAmount = j.at("Amounts").at("textures").get<int>();
    for(int i = 0; i < texturesAmount; ++i)
    {
        name = setName("texture", i);
        texCreateInfo.generateMipmaps = j.at(name).at("creationInfo").at("generateMipmaps").get<bool>();
        texCreateInfo.magFilter = GLenum(j.at(name).at("creationInfo").at("magFilter").get<int>());
        texCreateInfo.minFilter = GLenum(j.at(name).at("creationInfo").at("minFilter").get<int>());
        texCreateInfo.wrapMode = GLenum(j.at(name).at("creationInfo").at("wrapMode").get<int>());

        std::string path = j.at(name).at("filePath").get<std::string>();
        auto tex = objModulePtr->newTexture(path.c_str(), texCreateInfo);
        tex->serializationID = j.at(name).at("serializationID").get<unsigned int>();
    }

}

void SceneReader::readCubemaps()
{
    int cubemapsAmount = j.at("Amounts").at("cubemaps").get<int>();
    std::string name;
    std::string backPath, frontPath, leftPath, rightPath, topPath, bottomPath;
    TextureCreateInfo texCreateInfo;
    for(int i = 0; i < cubemapsAmount; ++i)
    {
        name = setName("cubemap", i);
        frontPath = j.at(name).at("frontPath").get<std::string>();
        bottomPath = j.at(name).at("bottomPath").get<std::string>();
        leftPath = j.at(name).at("leftPath").get<std::string>();
        rightPath = j.at(name).at("rightPath").get<std::string>();
        topPath = j.at(name).at("topPath").get<std::string>();
        backPath = j.at(name).at("backPath").get<std::string>();

        texCreateInfo.generateMipmaps = j.at(name).at("creationInfo").at("generateMipmaps").get<bool>();
        texCreateInfo.magFilter = GLenum(j.at(name).at("creationInfo").at("magFilter").get<int>());
        texCreateInfo.minFilter = GLenum(j.at(name).at("creationInfo").at("minFilter").get<int>());
        texCreateInfo.wrapMode = GLenum(j.at(name).at("creationInfo").at("wrapMode").get<int>());

        auto cubemap = objModulePtr->newCubemap(texCreateInfo, frontPath.c_str(), leftPath.c_str(), rightPath.c_str(), backPath.c_str(), topPath.c_str(), bottomPath.c_str());
        cubemap->serializationID = j.at(name).at("serializationID").get<unsigned int>();
    }
}

void SceneReader::readMaterials()
{
    std::string name;
    int materialsAmount = j.at("Amounts").at("materials").get<int>();
    std::unordered_map<std::string, unsigned int> children;
    unsigned int shaderID;

    for(int i = 0; i < materialsAmount; ++i)
    {
        name = setName("material", i);
        children.clear();

        auto matName = j.at(name).at("name").get<std::string>();

        shaderID = j.at(name).at("shaderSerializationID").get<unsigned int>();
        auto shader = objModulePtr->objectContainer.getShaderFromSerializationID(shaderID);

        auto material = objModulePtr->newMaterial(shader, matName);
        material->serializationID = j.at(name).at("serializationID").get<unsigned int>();

        j.at(name).at("cubemaps").get_to(children);
        for(auto iter : children)
        {
            unsigned int cubemapID = iter.second;
            auto cubemap = objModulePtr->objectContainer.getCubemapFromSerializationID(cubemapID);
            material->setCubemap(iter.first, cubemap);
        }

        children.clear();
        j.at(name).at("textures").get_to(children);
        for(auto iter : children)
        {
            unsigned int texID = iter.second;
            auto texture = objModulePtr->objectContainer.getTextureFromSerializationID(texID);
            material->setTexture(iter.first, texture);
        }
    }
}

void SceneReader::readMeshes()
{
    std::string name;
    int meshesAmount = j.at("Amounts").at("meshes");
    std::string filePath, meshPath;
    for(int i = 0; i < meshesAmount; ++i)
    {
        name = setName("mesh", i);
        filePath = j.at(name).at("filePath").get<std::string>();
        meshPath = j.at(name).at("meshPath").get<std::string>();

        auto meshType = j.at(name).at("type").get<std::string>();
        if(meshType == "MeshCustom")
        {
            objModulePtr->newModel(filePath.c_str(), FileType::MESH);
            auto mesh = objModulePtr->getMeshCustomFromPath(meshPath.c_str());
            mesh->serializationID = j.at(name).at("serializationID").get<unsigned int>();
        }
        else if(meshType == "MeshSkinned")
        {
            objModulePtr->newModel(filePath.c_str(), FileType::SKINNEDMESH);
            auto mesh = objModulePtr->getMeshSkinnedFromPath(meshPath.c_str());
            mesh->serializationID = j.at(name).at("serializationID").get<unsigned int>();
        }
        else;
    }
}

void SceneReader::readComponents()
{
git 
}

void SceneReader::readEntities()
{

}

std::string SceneReader::setName(std::string name, int index)
{
    if(index < 10)
    {
        return name += "00" + std::to_string(index);
    }
    else if(index < 100)
    {
        return name += "0" + std::to_string(index);
    }
    else
    {
        return name += std::to_string(index);
    }
    
}