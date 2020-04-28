#include "ObjectContainer.hpp"
#include "ObjectModule.hpp"
#include "ObjectMaker.hpp"

#include "Entity.hpp"
#include "Component.inl"
#include "Shader.hpp"
#include "Cubemap.hpp"
#include "Material.hpp"
#include "Transform.inl"
#include "Mesh.inl"

ObjectContainer::ObjectContainer(ObjectModule* objModule) : objModule(objModule) 
{
    // shaders.reserve(50);
    // components.reserve(100);
    // entities.reserve(50);
    // meshes.reserve(50);
    // textures.reserve(50);
}

ObjectContainer::ObjectContainer()
{

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
    shaders.clear();
    textures.clear();
}