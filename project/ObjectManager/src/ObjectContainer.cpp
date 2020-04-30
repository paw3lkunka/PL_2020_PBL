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
    components.reserve(100);
    entities.reserve(50);
    meshes.reserve(50);
    textures.reserve(50);
    cubemaps.reserve(10);
    materials.reserve(20);
}

ObjectContainer::ObjectContainer()
{
    shaders.reserve(50);
    components.reserve(100);
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