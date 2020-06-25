#ifndef IKBONEPOINT_INL_
#define IKBONEPOINT_INL_

#include "Component.hpp"

struct Transform;
struct Skeleton;

/**
 * @brief IK bone attachment point (settings for IK system)
 */
struct IKBonePoint : public Component
{   
    ///@brief chain length of bones
    int chainLength;

    ///@brief target the chain should bend to
    Transform* touchPoint = nullptr;

    ///@brief solver iterations per update
    int iterations = 30;

    ///@brief distance when the solver stops
    float delta = 0.01f;

    ///@brief strength of going back to the start position
    float snapBackStrength = 1.0f;

    ///@brief skeleton reference for global inverse transform
    Skeleton* skeletonPtr;
};

#endif /* !IKBONEPOINT_INL_ */
