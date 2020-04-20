#include "ObjectModule.hpp"

#include "Entity.hpp"
#include "Component.inl"
#include "System.hpp"

#include "Material.hpp"
#include "Shader.hpp"

int ObjectModule::nextID = 0;

ObjectModule::ObjectModule(int componentsSpace)
:   components(componentsSpace)
{
    //HACK resize of vectors may break pointers, so to prevent this, I temporarily allocate big space in constructor xD
    //!THIS IS ULTRA-TEMPORARY SOLUTION, AND SHOULD BE CHANGED ASAP.
    entities.reserve(200);
    materials.reserve(200);
    shaders.reserve(200);
}

void ObjectModule::NewEntity(int bufferSize = 0)
{
    entities.push_back( Entity(nextID++, bufferSize) );
}