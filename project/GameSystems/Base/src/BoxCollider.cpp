#include "BoxCollider.hpp"

#include "Core.hpp"
#include "Utils.hpp"

BoxCollider::BoxCollider()
{
    this->center   = {0.0f, 0.0f, 0.0f};
    this->halfSize = {0.5f, 0.5f, 0.5f};
}

BoxCollider::BoxCollider(glm::vec3 size, glm::vec3 center)
{
    this->center = center;
    this->halfSize = size * 0.5f;
}

BoxCollider::BoxCollider(float width, float height, float depth)    
{
    this->center = {0.0f, 0.0f, 0.0f};
    this->halfSize = {width * 0.5f, height * 0.5f, depth * 0.5f};
}

BoxCollider::BoxCollider(float width, float height, float depth, float x, float y, float z)
{
    this->center = {x,y,z};
    this->halfSize = {width * 0.5f, height * 0.5f, depth * 0.5f};
}

BoxCollider::~BoxCollider()
{
    GetCore().physicsCommon.destroyBoxShape(dynamic_cast<reactphysics3d::BoxShape*>(reactCS));
}

void BoxCollider::computeReactCS()
{
    //TODO Scale?
    reactCS = GetCore().physicsCommon.createBoxShape(Vec3Cast(halfSize));
}