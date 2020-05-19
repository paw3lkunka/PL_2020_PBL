#include "SkeletonSystem.hpp"

#include "Skeleton.inl"
#include "Transform.inl"
#include "Core.hpp"
#include "Entity.hpp"

bool SkeletonSystem::assertEntity(Entity* entity)
{
    skeleton = entity->getComponentPtr<Skeleton>();
    return skeleton != nullptr;
}

void SkeletonSystem::start()
{
    GetCore().getMessageBus().sendMessage(Message(Event::RENDERER_SET_BONE_TRANSFORMS_PTR, &boneTransforms));
    GetCore().getMessageBus().notify();
}

void SkeletonSystem::fixedUpdate()
{
    animationTime += Core::FIXED_TIME_STEP * 16.0f;
}

void SkeletonSystem::frameUpdate()
{
    processHierarchy(skeleton->rootBone);
}

void SkeletonSystem::processHierarchy(Entity* boneEntity)
{
    Transform* boneTransform = boneEntity->getComponentPtr<Transform>();

    boneTransforms[boneEntity->getComponentPtr<Bone>()->boneID] = skeleton->globalInverseTransform * boneTransform->modelMatrix * boneEntity->getComponentPtr<Bone>()->offsetMatrix;

    if(skeleton->animation != nullptr)
    {
        boneTransform->getLocalPositionModifiable() = skeleton->animation->getPositionLerp(boneEntity->getComponentPtr<Bone>()->boneID, animationTime);
        boneTransform->getLocalRotationModifiable() = skeleton->animation->getRotationLerp(boneEntity->getComponentPtr<Bone>()->boneID, animationTime);
        boneTransform->getLocalScaleModifiable() = skeleton->animation->getScaleLerp(boneEntity->getComponentPtr<Bone>()->boneID, animationTime);
    }

    for(auto child : boneEntity->getComponentPtr<Transform>()->children)
    {
        processHierarchy(child->entityPtr);
    }
}