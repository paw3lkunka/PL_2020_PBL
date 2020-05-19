#include "SkeletonSystem.hpp"

#include "Skeleton.inl"
#include "Bone.inl"
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

void SkeletonSystem::processHierarchy(Bone* bone)
{
    //TODO get back to it
    // if (bone->parent != nullptr)
    // {
    //     if (skeleton->animation != nullptr)
    //     {
    //         bone->boneTransform = bone->parent->boneTransform * skeleton->animation->getAnimTransformLerp(bone->boneID, animationTime); 
    //     }
    //     else
    //     {
    //         bone->boneTransform = bone->parent->boneTransform * bone->localBoneTransform;
    //     }
    // }
    // else
    // {
    //     if (skeleton->animation != nullptr)
    //     {
    //         bone->boneTransform = skeleton->animation->getAnimTransformLerp(bone->boneID, animationTime);
    //     }
    //     else
    //     {
    //         bone->boneTransform = bone->localBoneTransform;
    //     }
    // }

    // boneTransforms[bone->boneID] = skeleton->globalInverseTransform * bone->boneTransform * bone->offsetMatrix;

    // for(auto child : bone->children)
    // {
    //     processHierarchy(child);
    // }
}