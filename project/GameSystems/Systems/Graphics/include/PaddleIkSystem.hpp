#ifndef PADDLEIKSYSTEM_HPP_
#define PADDLEIKSYSTEM_HPP_

#include "System.hpp"
#include <string>
#include <map>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include "IKBonePoint.inl"
struct Transform;
struct Bone;

/**
 * @brief struct that represents one IK bone, where start is parent of end
 * 
 *              length
 *     |-------------------|
 *  endPos ------->    startPos
 *          direction
 */
struct IKBone
{
    ///@brief parent of end; equals to end of the bone before
    glm::vec3 startPos;
    ///@brief tip of bone which is startPos of the next Bone
    glm::vec3 endPos;
    ///@brief rotation of bone in world
    glm::quat rotation;
    ///@brief start rotation of bone in world
    glm::quat startRot;
    ///@brief normalized vector from end to start
    glm::vec3 dir;
    ///@brief length of bone
    float length;
    ///@brief pointer to bone transform to get and set position and rotation;
    Transform* boneTrans;
    ///@brief pointer to bone transform which is end of bone
    Transform* tipBoneTrans;
};

/**
 * @brief struct that represents chain of IK Bones
 */
struct IKChain
{
    ///@brief array of pointers to bones; size: chainLength; last bone is bone pointing at Target
    IKBone** bones;
    ///@brief length of chain (in bones)
    int chainLength;
    ///@brief length of chain (as a sum of bones Lengths)
    float completeBonesLength;
    ///@brief position of root Bone
    Transform* rootTrans;

    IKChain() = default;
    IKChain(IKBone** b, int cL, float cBL, Transform* rT) : bones(b), chainLength(cL), completeBonesLength(cBL), rootTrans(rT) {}
};

class PaddleIkSystem : public System
{
public:
    PaddleIkSystem() = default;
    ~PaddleIkSystem() = default;

    /**
     * @brief Checks, if given Entity contain all required components
     * implementation should also save pointers to components in class variables 
     * @param entity pointer to asserted entity
     * @returns if entity has all required components
     */
    virtual bool assertEntity(Entity* entity);

    /**
     * @brief Initializes processed entities, runs before first update
     */
    virtual void start();

    /**
     * @brief Contain logic of the system called in fixed time steps
     * called in Process() only when AssertEntity() returned true
     * should use class variables to access components
     */
    virtual void fixedUpdate() {}

    /**
     * @brief Contain logic of the system runned once per frame
     * called in Process() only when AssertEntity() returned true
     * should use class variables to access components
     */
    virtual void frameUpdate();

    /**
     * @brief Initialize structure map
     */
    void init();

protected:
private:

    void resolveIK();
    void resolveIKv2();
    void solve(Transform* endEffector, glm::vec4 target, size_t numIterations, float threshold, int numParents);

    Transform* endBonePtr;
    IKBonePoint* bonePointPtr;
    Bone* bonePtr;

    std::map<unsigned int, IKChain> chainMappings;

    // glm::vec3 getPositionRootSpace(Transform* current, Transform* root);
    // void setPositionRootSpace(Transform* current, glm::vec3 position, Transform* root);
    // glm::quat getRotationRootSpace(Transform* current, Transform* root);
    // void setRotationRootSpace(Transform* current, glm::quat rotation, Transform* root);

    glm::vec3 positionInWorld(Transform* trans);
    glm::vec3 positionInLocal(Transform* trans, glm::vec3 pos);

    glm::quat rotationInWorld(Transform* current);
    glm::quat rotationInLocal(Transform* current, glm::quat rot);

    glm::quat rotateVectors(glm::vec3 from, glm::vec3 to);
};

#endif /* !CANOEISTIKSYSTEM_HPP_ */
