#include "Animation.hpp"

Animation::Animation(std::vector<KeyVector> pKeys, std::vector<KeyQuaternion> rKeys, std::vector<KeyVector> sKeys, AnimationBehaviour bs, AnimationBehaviour as)
{
    positionKeys = pKeys;
    rotationKeys = rKeys;
    scaleKeys = sKeys;
    beforeState = bs;
    afterState = as;
}

void Animation::setBoneMappingMap(std::unordered_map<std::string, BoneInfo> map)
{
    boneMapping = map;
}