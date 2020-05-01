#include "SceneWriter.hpp"
#include "Components.inc"
#include "ObjectModule.hpp"
#include "ObjectContainer.hpp"

#include "Material.hpp"
#include "Texture.hpp"
#include "Cubemap.hpp"
#include "MeshCustom.hpp"
#include "MeshSkinned.hpp"
#include "Shader.hpp"
#include "Entity.hpp"

#include <fstream>
#include <iomanip>

SceneWriter::SceneWriter(ObjectModule* objectModulePtr)
{
    objModulePtr = objectModulePtr;
    objContainerPtr = &objectModulePtr->objectContainer;
}

SceneWriter::~SceneWriter()
{
    delete objModulePtr;
    objModulePtr = nullptr;
    
    delete objContainerPtr;
    objContainerPtr = nullptr;
}

void SceneWriter::saveScene(const char* filePath)
{
    std::string name;
    j["Amounts"]["entities"] = objContainerPtr->entities.size();
    j["Amounts"]["shaders"] = objContainerPtr->shaders.size();
    j["Amounts"]["materials"] = objContainerPtr->materials.size();
    j["Amounts"]["components"] = objContainerPtr->components.size();
    j["Amounts"]["meshes"] = objContainerPtr->meshes.size();
    j["Amounts"]["textures"] = objContainerPtr->textures.size();
    j["Amounts"]["cubemaps"] = objContainerPtr->cubemaps.size();
    
    for( int i = 0; i < objContainerPtr->entities.size(); ++i)
    {
        childrenID.clear();
        if(i < 10)
        {
            name = "entity00" + std::to_string(i);
        }
        else if(i < 100)
        {
            name = "entity0" + std::to_string(i);
        }
        else
        {
            name = "entity" + std::to_string(i);
        }
        j[name]["id"] = objContainerPtr->entities[i].getId();
        j[name]["serializationID"] = objContainerPtr->entities[i].serializationID;
        for(int j = 0; j < objContainerPtr->entities[i].getComponentsPtr()->size(); ++j)
        {
            childrenID.push_back(objContainerPtr->entities[i].getComponentsPtr()->operator[](j)->serializationID);
        }
        j[name]["components"] = childrenID;
    }

    for(int i = 0; i < objContainerPtr->shaders.size(); ++i)
    {
        if(i < 10)
        {
            name = "shader00" + std::to_string(i);
        }
        else if(i < 100)
        {
            name = "shader0" + std::to_string(i);
        }
        else
        {
            name = "shader" + std::to_string(i);
        }
        saveShader(name, objContainerPtr->shaders[i]);
    }

    for(int i = 0; i < objContainerPtr->materials.size(); ++i)
    {
        if(i < 10)
        {
            name = "material00" + std::to_string(i);
        }
        else if(i < 100)
        {
            name = "material0" + std::to_string(i);
        }
        else
        {
            name = "material" + std::to_string(i);
        }
        saveMaterial(name, objContainerPtr->materials[i]);
    }

    for(int i = 0; i < objContainerPtr->meshes.size(); ++i)
    {
        if(i < 10)
        {
            name = "mesh00" + std::to_string(i);
        }
        else if(i < 100)
        {
            name = "mesh0" + std::to_string(i);
        }
        else
        {
            name = "mesh" + std::to_string(i);
        }
        saveMesh(name, objContainerPtr->meshes[i]);
    }

    for(int i = 0; i < objContainerPtr->textures.size(); ++i)
    {
        if(i < 10)
        {
            name = "texture00" + std::to_string(i);
        }
        else if(i < 100)
        {
            name = "texture0" + std::to_string(i);
        }
        else
        {
            name = "texture" + std::to_string(i);
        }
        saveTexture(name, objContainerPtr->textures[i]);
    }

    for(int i = 0; i < objContainerPtr->cubemaps.size(); ++i)
    {
        if(i < 10)
        {
            name = "cubemap00" + std::to_string(i);
        }
        else if(i < 100)
        {
            name = "cubemap0" + std::to_string(i);
        }
        else
        {
            name = "cubemap" + std::to_string(i);
        }
        saveCubemap(name, objContainerPtr->cubemaps[i]);
    }

    for( int i = 0; i < objContainerPtr->components.size(); ++i)
    {
        if(i < 10)
        {
            name = "component00" + std::to_string(i);
        }
        else if(i < 100)
        {
            name = "component0" + std::to_string(i);
        }
        else
        {
            name = "component" + std::to_string(i);
        }

        j[name]["entity id"] = objContainerPtr->components[i]->entityPtr->getId();
        j[name]["serializationID"] = objContainerPtr->components[i]->serializationID;
        if(dynamic_cast<Transform*>(objContainerPtr->components[i]))
        {
            Transform* temp = dynamic_cast<Transform*>(objContainerPtr->components[i]);
            saveTransform(name, temp);
        }
        else if(dynamic_cast<Bone*>(objContainerPtr->components[i]))
        {
            Bone* temp = dynamic_cast<Bone*>(objContainerPtr->components[i]);
            saveBone(name, temp);
        }
        else if(dynamic_cast<AudioSource*>(objContainerPtr->components[i]))
        {
            AudioSource* temp = dynamic_cast<AudioSource*>(objContainerPtr->components[i]);
            saveAudioSource(name, temp);
        }
        else if(dynamic_cast<AudioListener*>(objContainerPtr->components[i]))
        {
            AudioListener* temp = dynamic_cast<AudioListener*>(objContainerPtr->components[i]);
            saveAudioListener(name, temp);
        }
        else if(dynamic_cast<Camera*>(objContainerPtr->components[i]))
        {
            Camera* temp = dynamic_cast<Camera*>(objContainerPtr->components[i]);
            saveCamera(name, temp);
        }
        else if(dynamic_cast<BillboardRenderer*>(objContainerPtr->components[i]))
        {
            BillboardRenderer* temp = dynamic_cast<BillboardRenderer*>(objContainerPtr->components[i]);
            saveBillboardRenderer(name, temp);
        }
        else if(dynamic_cast<Renderer*>(objContainerPtr->components[i]))
        {
            Renderer* temp = dynamic_cast<Renderer*>(objContainerPtr->components[i]);
            saveRenderer(name, temp);
        }
        else if(dynamic_cast<SphereCollider*>(objContainerPtr->components[i]))
        {
            SphereCollider* temp = dynamic_cast<SphereCollider*>(objContainerPtr->components[i]);
            saveSphereCollider(name, temp);
        }
    }

    std::ofstream file(filePath);
    if(file.good())
    {
        file << std::setw(4) << j;
    }
    file.close();
}

void SceneWriter::saveBone(std::string name, Bone* componentPtr)
{
    j[name]["type"] = "Bone";
    j[name]["filePath"] = componentPtr->filePath;
}

void SceneWriter::saveTransform(std::string name, Transform* componentPtr)
{
    j[name]["type"] = "Transform";
    if(componentPtr->getParent() != nullptr)
        j[name]["transform parentID"] = componentPtr->getParent()->serializationID;
    j[name]["localPosition"]["x"] = componentPtr->getLocalPosition().x;
    j[name]["localPosition"]["y"] = componentPtr->getLocalPosition().y;
    j[name]["localPosition"]["z"] = componentPtr->getLocalPosition().z;
    j[name]["localRotation"]["x"] = componentPtr->getLocalRotation().x;
    j[name]["localRotation"]["y"] = componentPtr->getLocalRotation().y;
    j[name]["localRotation"]["z"] = componentPtr->getLocalRotation().z;
    j[name]["localRotation"]["w"] = componentPtr->getLocalRotation().w;
    j[name]["localScale"]["x"] = componentPtr->getLocalScale().x;
    j[name]["localScale"]["y"] = componentPtr->getLocalScale().y;
    j[name]["localScale"]["z"] = componentPtr->getLocalScale().z;
}

void SceneWriter::saveAudioListener(std::string name, AudioListener* componentPtr)
{
    j[name]["type"] = "AudioListener";
    j[name]["gain"] = componentPtr->getGain();
    j[name]["at"]["x"] = componentPtr->getAt().x;
    j[name]["at"]["y"] = componentPtr->getAt().y;
    j[name]["at"]["z"] = componentPtr->getAt().z;
    j[name]["up"]["x"] = componentPtr->getUp().x;
    j[name]["up"]["y"] = componentPtr->getUp().y;
    j[name]["up"]["z"] = componentPtr->getUp().z;
    j[name]["isCurrent"] = componentPtr->getIsCurrent();
}

void SceneWriter::saveAudioSource(std::string name, AudioSource* componentPtr)
{
    j[name]["type"] = "AudioSource";
    childrenID.clear();
    for(int j = 0; j < componentPtr->getListenersModifiable().size(); ++j)
    {
        childrenID.push_back(componentPtr->getListenersModifiable().at(j)->serializationID);
    }
    j[name]["listeners"] = childrenID; 
    j[name]["clips"] = componentPtr->getClips();
    j[name]["isRelativeToListener"] = componentPtr->getIsRelative();
    j[name]["isLooping"] = componentPtr->getIsLooping();
    j[name]["minGain"] = componentPtr->getMinGain();
    j[name]["maxGain"] = componentPtr->getMaxGain();
    j[name]["referenceDistance"] = componentPtr->getReferenceDistance();
    j[name]["rolloffFactor"] = componentPtr->getRolloffFactor();
    if(componentPtr->getMaxDistance() != std::numeric_limits<float>::infinity())
    {
        j[name]["maxDistance"] = componentPtr->getMaxDistance();
    }
    else
    {
        j[name]["maxDistance"] = "infinity";
    }
    j[name]["pitch"] = componentPtr->getPitch();
    j[name]["direction"]["x"] = componentPtr->getDirection().x;
    j[name]["direction"]["y"] = componentPtr->getDirection().y;
    j[name]["direction"]["z"] = componentPtr->getDirection().z;
    j[name]["coneInnerAngle"] = componentPtr->getConeInnerAngle();
    j[name]["coneOuterAngle"] = componentPtr->getConeOuterAngle();
    j[name]["coneOuterGain"] = componentPtr->getConeOuterGain();
}

void SceneWriter::saveCamera(std::string name, Camera* componentPtr)
{
    j[name]["type"] = "Camera";
    j[name]["projectionMode"] = componentPtr->projectionMode;
    j[name]["isMain"] = componentPtr->isMain;
    j[name]["fieldOfView"] = componentPtr->fieldOfView;
    j[name]["orthographicSize"] = componentPtr->orthographicSize;
    j[name]["nearPlane"] = componentPtr->nearPlane;
    j[name]["farPlane"] = componentPtr->farPlane;
}

void SceneWriter::saveRenderer(std::string name, Renderer* componentPtr)
{
    j[name]["type"] = "Renderer";
    j[name]["material"] = componentPtr->material->serializationID;
    j[name]["mesh"] = componentPtr->mesh->serializationID;
    
}

void SceneWriter::saveBillboardRenderer(std::string name, BillboardRenderer* componentPtr)
{
    j[name]["type"] = "BillboardRenderer";
    j[name]["material"] = componentPtr->material->serializationID;
}

void SceneWriter::saveSphereCollider(std::string name, SphereCollider* componentPtr)
{
    j[name]["type"] = "SphereCollider";
    j[name]["colliderType"] = componentPtr->type;
    j[name]["radius"] = componentPtr->radius;
    j[name]["center"]["x"] = componentPtr->center.x;
    j[name]["center"]["y"] = componentPtr->center.y;
    j[name]["center"]["z"] = componentPtr->center.z;
}

void SceneWriter::saveMaterial(std::string name, Material* assetPtr)
{
    j[name]["serializationID"] = assetPtr->serializationID;
    j[name]["shaderSerializationID"] = assetPtr->shader->serializationID;
    j[name]["name"] = assetPtr->getName();
    childrenMap.clear();
    for(auto c : assetPtr->cubemaps)
    {
        childrenMap[c.first] = c.second->serializationID;
    }
    j[name]["cubemaps"] = childrenMap;

    childrenMap.clear();
    for(auto t : assetPtr->textures)
    {
        childrenMap[t.first] = t.second->serializationID;
    }
    j[name]["textures"] = childrenMap;
}

void SceneWriter::saveTexture(std::string name, Texture* assetPtr)
{
    j[name]["serializationID"] = assetPtr->serializationID;
    j[name]["filePath"] = assetPtr->filePath;
    j[name]["creationInfo"]["generateMipmaps"] = assetPtr->info.generateMipmaps;
    j[name]["creationInfo"]["minFilter"] = assetPtr->info.minFilter;
    j[name]["creationInfo"]["magFilter"] = assetPtr->info.magFilter;
    j[name]["creationInfo"]["wrapMode"] = assetPtr->info.wrapMode;
}

void SceneWriter::saveMesh(std::string name, Mesh* assetPtr)
{
    j[name]["serializationID"] = assetPtr->serializationID;
    j[name]["meshPath"] = assetPtr->getMeshPath();
    j[name]["filePath"] = assetPtr->filePath;
    if(dynamic_cast<MeshCustom*>(assetPtr) != nullptr)
    {
        j[name]["type"] = "MeshCustom";
    }
    else if(dynamic_cast<MeshSkinned*>(assetPtr) != nullptr)
    {
        j[name]["type"] = "MeshSkinned";
    }
}

void SceneWriter::saveShader(std::string name, Shader* assetPtr)
{
    j[name]["serializationID"] = assetPtr->serializationID;
    j[name]["fragmentShaderPath"] = assetPtr->fragmentShaderPath;
    j[name]["vertexShaderPath"] = assetPtr->vertexShaderPath;
    if(assetPtr->geometryShaderPath != "")
    {
        j[name]["geometryShaderPath"] = assetPtr->geometryShaderPath;
    }
}

void SceneWriter::saveCubemap(std::string name, Cubemap* assetPtr)
{
    j[name]["serializationID"] = assetPtr->serializationID;
    j[name]["frontPath"] = assetPtr->frontPath;
    j[name]["backPath"] = assetPtr->backPath;
    j[name]["leftPath"] = assetPtr->leftPath;
    j[name]["rightPath"] = assetPtr->rightPath;
    j[name]["topPath"] = assetPtr->topPath;
    j[name]["bottomPath"] = assetPtr->bottomPath;
    j[name]["creationInfo"]["generateMipmaps"] = assetPtr->info.generateMipmaps;
    j[name]["creationInfo"]["minFilter"] = assetPtr->info.minFilter;
    j[name]["creationInfo"]["magFilter"] = assetPtr->info.magFilter;
    j[name]["creationInfo"]["wrapMode"] = assetPtr->info.wrapMode;
}