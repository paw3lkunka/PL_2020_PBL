#include "SceneWriter.hpp"
#include "Components.inc"
#include "ObjectModule.hpp"
#include "ObjectContainer.hpp"

#include "Material.hpp"
#include "Texture.hpp"
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
    std::unordered_map<std::string, unsigned int> childrenMap;
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
        j[name]["serialization ID"] = objContainerPtr->entities[i].serializationID;
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
        j[name]["serializationID"] = objContainerPtr->shaders[i]->serializationID;
        j[name]["fragmentShaderPath"] = objContainerPtr->shaders[i]->fragmentShaderPath;
        j[name]["vertexShaderPath"] = objContainerPtr->shaders[i]->vertexShaderPath;
        if(objContainerPtr->shaders[i]->geometryShaderPath != "")
        {
            j[name]["geometryShaderPath"] = objContainerPtr->shaders[i]->geometryShaderPath;
        }
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
        j[name]["serializationID"] = objContainerPtr->materials[i]->serializationID;
        j[name]["shaderSerializationID"] = objContainerPtr->materials[i]->shader->serializationID;
        childrenMap.clear();
        for(auto c : objContainerPtr->materials[i]->cubemaps)
        {
            childrenMap[c.first] = c.second->serializationID;
        }
        j[name]["cubemaps"] = childrenMap;

        childrenMap.clear();
        for(auto t : objContainerPtr->materials[i]->textures)
        {
            childrenMap[t.first] = t.second->serializationID;
        }
        j[name]["textures"] = childrenMap;
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
        j[name]["serializationID"] = objContainerPtr->meshes[i]->serializationID;
        j[name]["meshPath"] = objContainerPtr->meshes[i]->getMeshPath();
        j[name]["filePath"] = objContainerPtr->meshes[i]->filePath;
        if(dynamic_cast<MeshCustom*>(objContainerPtr->meshes[i]) != nullptr)
        {
            j[name]["type"] = "MeshCustom";
        }
        else if(dynamic_cast<MeshSkinned*>(objContainerPtr->meshes[i]) != nullptr)
        {
            j[name]["type"] = "MeshSkinned";
        }
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
        j[name]["serializationID"] = objContainerPtr->textures[i]->serializationID;
        j[name]["filePath"] = objContainerPtr->textures[i]->filePath;
        j[name]["creationInfo"]["generateMipmaps"] = objContainerPtr->textures[i]->info.generateMipmaps;
        j[name]["creationInfo"]["minFilter"] = objContainerPtr->textures[i]->info.minFilter;
        j[name]["creationInfo"]["magFilter"] = objContainerPtr->textures[i]->info.magFilter;
        j[name]["creationInfo"]["wrapMode"] = objContainerPtr->textures[i]->info.wrapMode;
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
        j[name]["serializationID"] = objContainerPtr->cubemaps[i]->serializationID;
        j[name]["frontPath"] = objContainerPtr->cubemaps[i]->frontPath;
        j[name]["backPath"] = objContainerPtr->cubemaps[i]->backPath;
        j[name]["leftPath"] = objContainerPtr->cubemaps[i]->leftPath;
        j[name]["rightPath"] = objContainerPtr->cubemaps[i]->rightPath;
        j[name]["topPath"] = objContainerPtr->cubemaps[i]->topPath;
        j[name]["bottomPath"] = objContainerPtr->cubemaps[i]->bottomPath;
        j[name]["creationInfo"]["generateMipmaps"] = objContainerPtr->cubemaps[i]->info.generateMipmaps;
        j[name]["creationInfo"]["minFilter"] = objContainerPtr->cubemaps[i]->info.minFilter;
        j[name]["creationInfo"]["magFilter"] = objContainerPtr->cubemaps[i]->info.magFilter;
        j[name]["creationInfo"]["wrapMode"] = objContainerPtr->cubemaps[i]->info.wrapMode;
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
        j[name]["serialization ID"] = objContainerPtr->components[i]->serializationID;
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