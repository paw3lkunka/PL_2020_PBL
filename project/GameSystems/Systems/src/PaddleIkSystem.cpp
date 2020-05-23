#include "PaddleIkSystem.hpp"
#include "Components.inc"
#include "Entity.hpp"
#include "Bone.inl"

void PaddleIkSystem::init(BoneAttachData left, BoneAttachData right, Skeleton* ikSkeleton)
{
    attachLeft = left;
    attachRight = right;
    skeleton = ikSkeleton;
}

bool PaddleIkSystem::assertEntity(Entity* entity)
{
    paddlePtr = entity->getComponentPtr<Paddle>();
    transformPtr = entity->getComponentPtr<Transform>();
    return (paddlePtr != nullptr && transformPtr != nullptr);
}

void PaddleIkSystem::frameUpdate()
{
    processAttach(attachLeft);
    processAttach(attachRight);
}

void PaddleIkSystem::processAttach(BoneAttachData data)
{
    // Bone* end = data.bone;
    // Transform* endTransform = data.attachEntityPtr->getComponentPtr<Transform>();
    // glm::mat4 localTrans = glm::mat4(1.0f);
    // localTrans = glm::translate(localTrans, endTransform->getLocalPosition());
    // localTrans = localTrans * glm::toMat4(endTransform->getLocalRotation());
    // localTrans = glm::scale(localTrans, endTransform->getLocalScale());
    // end->localBoneTransform = endTransform->parentMatrix * localTrans;
}