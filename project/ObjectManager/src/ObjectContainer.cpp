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

ObjectContainer::ObjectContainer(ObjectModule* objModule) : objModule(objModule) 
{
    shaders.reserve(50);
    components.reserve(200);
    entities.reserve(50);
    meshes.reserve(50);
    textures.reserve(50);
    cubemaps.reserve(10);
    materials.reserve(20);
}

ObjectContainer::ObjectContainer()
{
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
    }
    components.clear();
    
    for(auto m : meshes)
    {
        delete m;
    }
    meshes.clear();

    entities.clear();
    for(auto s : shaders)
    {
        delete s;
    }
    shaders.clear();

    for(auto t : textures)
    {
        delete t;
    }
    textures.clear();

    for(auto m : materials)
    {
        delete m;
    }
    materials.clear();

    for(auto c : cubemaps)
    {
        delete c;
    }
    cubemaps.clear();
}

MeshCustom* ObjectContainer::getMeshCustomFromPath(const char* meshPath)
{
    for(auto m : meshes)
    {
        if(m->getMeshPath() == meshPath && dynamic_cast<MeshCustom*>(m) != nullptr)
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
        if(m->getMeshPath() == meshPath && dynamic_cast<MeshSkinned*>(m) != nullptr)
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
        if(m->getName() == name)
        {
            return m;
        }
    }
    return nullptr;
}