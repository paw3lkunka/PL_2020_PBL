#ifndef ANIMATION_HPP_
#define ANIMATION_HPP_

#include <unordered_map>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Bone.inl"

/**
 * @brief Defines how an animation behaves outside the defined time range
 */
enum class AnimationBehaviour
{
    ///@brief Value from the default node transformation
    DEFAULT = 0,
    ///@brief Nearest key value without interpolation
    CONSTANT = 1,
    ///@brief Nearest two keys are linearly extrapolated
    LINEAR,
    ///@brief The animation is repeated
    REPEAT,
};

struct AnimationNode
{
    AnimationNode() = default;
    virtual ~AnimationNode() = default;

    // ! ===== Warning, the Animation class assumes that these keyframes are sorted =====
    // ! ===== I'm not using std::map here because doubles as keys are problematic =====
    std::vector<std::pair<double, glm::vec3>> positions;
    std::vector<std::pair<double, glm::quat>> rotations;
    std::vector<std::pair<double, glm::vec3>> scales;
};


class Animation
{
public:
    Animation() = default;
    virtual ~Animation() = default;

    AnimationNode* addNode(unsigned int boneID);
    void addPositionKey(unsigned int boneID, double time, glm::vec3 position);
    void addRotationKey(unsigned int boneID, double time, glm::quat rotation);
    void addScaleKey(unsigned int boneID, double time, glm::vec3 scale);
    glm::mat4 getAnimTransformLerp(unsigned int boneID, double time);
    AnimationNode* findAnimNode(unsigned int boneID);
private:
    glm::vec3 getPositionLerp(AnimationNode &node, double time);
    glm::quat getRotationLerp(AnimationNode &node, double time);
    glm::vec3 getScaleLerp(AnimationNode &node, double time);

    std::unordered_map<unsigned int, AnimationNode> animationNodes;
};



#endif /* !ANIMATION_HPP_ */
