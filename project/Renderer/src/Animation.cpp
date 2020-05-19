#include "Animation.hpp"

#include <glm/gtc/quaternion.hpp>

AnimationNode* Animation::addNode(unsigned int boneID)
{
    animationNodes.insert({boneID, AnimationNode()});
    return &animationNodes[boneID];
}

void Animation::addPositionKey(unsigned int boneID, double time, glm::vec3 position)
{
    animationNodes[boneID].positions.push_back({time, position});
}

void Animation::addRotationKey(unsigned int boneID, double time, glm::quat rotation)
{
    animationNodes[boneID].rotations.push_back({time, rotation});
}

void Animation::addScaleKey(unsigned int boneID, double time, glm::vec3 scale)
{
    animationNodes[boneID].scales.push_back({time, scale});
}

AnimationNode* Animation::findAnimNode(unsigned int boneID)
{
    // TODO: When node is not found the behaviour may be undefined
    return &animationNodes[boneID];
}

glm::mat4 Animation::getAnimTransformLerp(unsigned int boneID, double time)
{
    auto animNode = animationNodes.find(boneID);
    glm::mat4 result = glm::mat4(1.0f);

    if (animNode != animationNodes.end())
    {
        // * ----- Requested node found, interpolate the animation -----
        glm::vec3 lerpPos = getPositionLerp(boneID, time);
        glm::quat lerpRot = getRotationLerp(boneID, time);
        glm::vec3 lerpScl = getScaleLerp(boneID, time);

        result = glm::translate(result, lerpPos);
        result = result * glm::toMat4(lerpRot);
        result = glm::scale(result, lerpScl);
    }
    
    return result;
}

glm::vec3 Animation::getPositionLerp(unsigned int boneID, double time)
{
    auto node = animationNodes.find(boneID)->second;
    while(time > node.animationLength)
    {
        time -= node.animationLength;
    }
    for (size_t i = 0; i < node.positions.size() - 1; i++)
    {
        if (time <= node.positions[i].first)
        {
            double interpolant = (time - node.positions[i - 1].first) / (node.positions[i].first - node.positions[i - 1].first);
            return glm::mix(node.positions[i - 1].second, node.positions[i].second, interpolant);
        }
    }
    // TODO: Handle unexpected behaviour
    return glm::vec3(1.0f);
}

glm::quat Animation::getRotationLerp(unsigned int boneID, double time)
{
    auto node = animationNodes.find(boneID)->second;
    while(time > node.animationLength)
    {
        time -= node.animationLength;
    }
    for (size_t i = 0; i < node.rotations.size() - 1; i++)
    {
        if (time <= node.rotations[i].first)
        {
            float interpolant = (time - node.rotations[i - 1].first) / (node.rotations[i].first - node.rotations[i - 1].first);
            return glm::slerp(node.rotations[i - 1].second, node.rotations[i].second, interpolant);
        }
    }
    // TODO: Handle unexpected behaviour
    return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
}

glm::vec3 Animation::getScaleLerp(unsigned int boneID, double time)
{
    auto node = animationNodes.find(boneID)->second;
    while(time > node.animationLength)
    {
        time -= node.animationLength;
    }
    for (size_t i = 0; i < node.scales.size() - 1; i++)
    {
        if (time <= node.scales[i].first)
        {
            double interpolant = (time - node.scales[i - 1].first) / (node.scales[i].first - node.scales[i - 1].first);
            return glm::mix(node.scales[i - 1].second, node.scales[i].second, interpolant);
        }
    }
    // TODO: Handle unexpected behaviour
    return glm::vec3(1.0f);
}