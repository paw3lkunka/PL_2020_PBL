#ifndef ANIMATION_HPP_
#define ANIMATION_HPP_

#include <vector>
#include <unordered_map>

#include "AnimationDataStructures.inl"
#include "MeshDataStructures.inl"
class Mesh;

/**
 * @brief 
 */
class Animation
{
public:
    /**
     * @brief Construct a new Animation object
     * 
     * @param pKeys vector of position keys
     * @param rKeys vector of rotation keys
     * @param sKeys vector of scale keys
     * @param bs animation behaviour before first key
     * @param as animation behaviour after last key
     */
    Animation(std::vector<KeyVector> pKeys, std::vector<KeyQuaternion> rKeys, std::vector<KeyVector> sKeys, AnimationBehaviour bs, AnimationBehaviour as);
    ~Animation() = default;

    void setBoneMappingMap(std::unordered_map<std::string, BoneInfo> map);

protected:
private:
    ///@brief position keys of this animation
    std::vector<KeyVector> positionKeys;
    ///@brief rotation keys of this animation
    std::vector<KeyQuaternion> rotationKeys;
    ///@brief scale keys of this animation
    std::vector<KeyVector> scaleKeys;
    ///@brief defines how the animation behaves before the first key
    AnimationBehaviour beforeState;
    ///@brief defines how the animation behaves after the last key
    AnimationBehaviour afterState;

    std::unordered_map<std::string, BoneInfo> boneMapping;
};

#endif /* !ANIMATION_HPP_ */
