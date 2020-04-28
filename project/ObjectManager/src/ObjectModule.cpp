#include "ObjectModule.hpp"

#include "Entity.hpp"

#include <assimp/scene.h>
#include <assimp/anim.h>

std::vector<Entity>* ObjectModule::getEntitiesVector()
{
    return &objectContainer.entities;
}

