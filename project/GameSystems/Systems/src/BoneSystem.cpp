#include "BoneSystem.hpp"

#include "Entity.hpp"

bool BoneSystem::assertEntity(Entity* entity)
{
    transform = entity->getComponentPtr<Transform>();
    bone = entity->getComponentPtr<Bone>();
    return (transform != nullptr && bone != nullptr);
}

void BoneSystem::fixedUpdate()
{
    // * ===== Interpolate between keyframes based on animationTime and save values to transform =====
    AnimationKey interpolatedKey = interpolateKeys(animationTime);
    transform->getLocalPositionModifiable() = interpolatedKey.position;
    transform->getLocalRotationModifiable() = interpolatedKey.rotation;
}

AnimationKey BoneSystem::interpolateKeys(double time)
{
    AnimationKey out;
    AnimationKey& min = bone->animationKeys.begin()->second;
    AnimationKey& max = bone->animationKeys.begin()->second;
    double lastTime = 0.0;
    double currentTime = 0.0;
    for(auto &key : bone->animationKeys)
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
    out.position = glm::mix(min.position, max.position, interpolator);
    out.rotation = glm::mix(min.rotation, max.rotation, interpolator);
    return out;
}