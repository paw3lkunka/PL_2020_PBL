#include "ObjectContainer.hpp"
#include "ObjectModule.hpp"
#include "ObjectMaker.hpp"
#include "ObjectExceptions.inl"

#include "Entity.hpp"
#include "Component.inl"
#include "Shader.hpp"
#include "Cubemap.hpp"
#include "Material.hpp"
#include "Transform.inl"
#include "Mesh.inl"
#include "MeshCustom.hpp"
#include "MeshSkinned.hpp"

bool ObjectContainer::hasInstance = false;

ObjectContainer::ObjectContainer(ObjectModule* objModule) : objModule(objModule) 
{
    if(hasInstance)
    {
        throw TooManyInstancesException("ObjectContainer");
    }
    hasInstance = true;
    shaders.reserve(50);
    components.reserve(200);
    entities.reserve(50);
    meshes.reserve(50);
    textures.reserve(50);
    cubemaps.reserve(10);
    materials.reserve(20);
}

ObjectContainer::~ObjectContainer()
{
    for(auto c : components)
    {
        delete c;
        c = nullptr;
    }
    components.clear();
    
    for(auto m : meshes)
    {
        delete m;
        m = nullptr;
    }
    meshes.clear();

    entities.clear();

    for(auto s : shaders)
    {
        delete s;
        s = nullptr;
    }
    shaders.clear();

    for(auto t : textures)
    {
        delete t;
        t = nullptr;
    }
    textures.clear();

    for(auto m : materials)
    {
        delete m;
        m = nullptr;
    }
    materials.clear();

    for(auto c : cubemaps)
    {
        delete c;
        c = nullptr;
    }
    cubemaps.clear();
}

MeshCustom* ObjectContainer::getMeshCustomFromPath(const char* meshPath)
{
    for(auto m : meshes)
    {
        if(objModule->compareStrings(m->getMeshPath().c_str(), meshPath) && dynamic_cast<MeshCustom*>(m) != nullptr)
        {
            return dynamic_cast<MeshCustom*>(m);
        }
    }
    throw FileNotFoundException(meshPath);
}

MeshSkinned* ObjectContainer::getMeshSkinnedFromPath(const char* meshPath)
{
    for(auto m : meshes)
    {
        if(objModule->compareStrings(m->getMeshPath().c_str(), meshPath) && dynamic_cast<MeshSkinned*>(m) != nullptr)
        {
            return dynamic_cast<MeshSkinned*>(m);
        }
    }
    throw FileNotFoundException(meshPath);
}

Entity* ObjectContainer::getEntityFromID(unsigned int entityID)
{
    for(int i = 0; i < entities.size(); ++i)
    {
        if(entities[i].getId() == entityID)
        {
            return &entities[i];
        }
    }
    return nullptr;
}

Entity* ObjectContainer::getEntityFromName(const char* name)
{
    for(int i = 0; i < entities.size(); ++i)
    {
        if(objModule->compareStrings(entities[i].getName().c_str(), name) )
        {
            return &entities[i];
        }
    }
    return nullptr;
}

Entity* ObjectContainer::getEntityFromSerializationID(unsigned int serializationID)
{
    for(int i = 0; i < entities.size(); ++i)
    {
        if(entities[i].serializationID == serializationID)
        {
            return &entities[i];
        }
    }
    return nullptr;
}

Component* ObjectContainer::getComponentFromSerializationID(unsigned int serializationID)
{
    for(auto c : components)
    {
        if(c->serializationID == serializationID)
        {
            return c;
        }
    }
    return nullptr;
}

Shader* ObjectContainer::getShaderFromSerializationID(unsigned int serializationID)
{
    for(auto s : shaders)
    {
        if(s->serializationID == serializationID)
        {
            return s;
        }
    }
    return nullptr;
}

Mesh* ObjectContainer::getMeshFromSerializationID(unsigned int serializationID)
{
    for(auto m : meshes)
    {
        if(m->serializationID == serializationID)
        {
            return m;
        }
    }
    return nullptr;
}

Texture* ObjectContainer::getTextureFromSerializationID(unsigned int serializationID)
{
    for(auto t : textures)
    {
        if(t->serializationID == serializationID)
        {
            return t;
        }
    }
    return nullptr;
}

Cubemap* ObjectContainer::getCubemapFromSerializationID(unsigned int serializationID)
{
    for(auto c : cubemaps)
    {
        if(c->serializationID == serializationID)
        {
            return c;
        }
    }
    return nullptr;
}

Material* ObjectContainer::getMaterialFromSerializationID(unsigned int serializationID)
{
    for(auto m : materials)
    {
        if(m->serializationID == serializationID)
        {
            return m;
        }
    }
    return nullptr;
}

Material* ObjectContainer::getMaterialFromName(const char* name)
{
    for(auto m : materials)
    {
        if(objModule->compareStrings(m->getName(), name))
        {
            return m;
        }
    }
    return nullptr;
}

Bone* ObjectContainer::getBonePtrByName(const char* name)
{
    return &bones.at(name);
}

unsigned int ObjectContainer::getBoneCount()
{
    return bones.size();
}

Animation* ObjectContainer::getAnimationPtrByName(const char* name)
{
    return &animations.at(name);
}