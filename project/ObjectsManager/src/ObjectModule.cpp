#include "ObjectModule.hpp"

#include "Entity.hpp"
#include "Component.inl"
#include "System.hpp"
#include "Material.hpp"
#include "Shader.hpp"
#include "Message.inl"
#include "Event.hpp"
#include "Transform.inl"

#include <assimp/scene.h>
#include <assimp/anim.h>

int ObjectModule::nextID = 0;

ObjectModule::ObjectModule()
{
    //HACK resize of vectors may break pointers, so to prevent this, I temporarily allocate big space in constructor xD
    //!THIS IS ULTRA-TEMPORARY SOLUTION, AND SHOULD BE CHANGED ASAP.
    entities.reserve(200);
    materials.reserve(200);
    shaders.reserve(200);
}

ObjectModule::~ObjectModule()
{
    //HACK remove as fast, as possible
    for(Component* component : components)
    {
        delete component;
    }
}

void ObjectModule::NewEntity(int bufferSize = 0)
{
    entities.push_back( Entity(nextID++, bufferSize) );
}

