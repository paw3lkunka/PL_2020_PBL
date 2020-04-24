#include "BoneSystem.hpp"

#include "Core.hpp"
#include "Entity.hpp"

bool BoneSystem::assertEntity(Entity* entity)
{
    transform = entity->getComponentPtr<Transform>();
    bone = entity->getComponentPtr<Bone>();
    return (transform != nullptr && bone != nullptr);
}

void BoneSystem::start()
{
    GetCore().getMessageBus().sendMessage(Message(Event::RENDERER_SET_BONE_TRANSFORMS_PTR, &boneTransforms));
    GetCore().getMessageBus().notify();
}

void BoneSystem::fixedUpdate()
{
    // * ===== Interpolate between keyframes based on animationTime and save values to transform =====
    glm::vec3 interpolatedPositionKey = interpolatePositionKeys(animationTime);
    glm::quat interpolatedRotationKey = interpolateRotationKeys(animationTime);
    transform->getLocalPositionModifiable() = interpolatedPositionKey;
    transform->getLocalRotationModifiable() = interpolatedRotationKey;
    animationTime += animationStep;
}

void BoneSystem::frameUpdate()
{
    // * ===== Retrieve interpolated bone transformations and set them  =====
    boneTransforms[bone->boneID] = transform->localToWorldMatrix;
}

glm::vec3 BoneSystem::interpolatePositionKeys(double time)
{
    glm::vec3 out;
    glm::vec3& min = bone->positionKeys.begin()->second;
    glm::vec3& max = bone->positionKeys.begin()->second;
    double lastTime = 0.0;
    double currentTime = 0.0;
    for(auto &key : bone->positionKeys)
    {
        max = key.second;
        currentTime = key.first;
        // TODO: Handle animation end
        if (lastTime <= time && currentTime > time)
        {
            break;
        }
        else
        {
            min = max;
            lastTime = currentTime;
        }
    }
    double interpolator = time - lastTime;
    out = glm::mix(min, max, interpolator);
    //out.rotation = glm::mix(min.rotation, max.rotation, interpolator);
    return out;
}

glm::quat BoneSystem::interpolateRotationKeys(double time)
{
    glm::quat out;
    glm::quat& min = bone->rotationKeys.begin()->second;
    glm::quat& max = bone->rotationKeys.begin()->second;
    double lastTime = 0.0;
    double currentTime = 0.0;
    for(auto &key : bone->rotationKeys)
    {
        max = key.second;
        currentTime = key.first;
        // TODO: Handle animation end
        if (lastTime <= time && currentTime > time)
        {
            break;
        }
        else
        {
            min = max;
            lastTime = currentTime;
        }
    }
    double interpolator = time - lastTime;
    out = glm::mix(min, max, (float)interpolator);
    return out;
}