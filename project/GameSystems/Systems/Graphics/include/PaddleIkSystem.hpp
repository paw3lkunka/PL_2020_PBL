#ifndef PADDLEIKSYSTEM_HPP_
#define PADDLEIKSYSTEM_HPP_

#include "System.hpp"
#include <string>

struct Transform;
struct Paddle;
struct Skeleton;
class Bone;

/**
 * @brief data struct for easier saving bone to attachPlace
 */
struct BoneAttachData
{   
    ///@brief end bone
    Entity* boneEntity;
    ///@brief entity with attach transform point
    Entity* attachEntityPtr;
};

class PaddleIkSystem : public System
{
public:
    PaddleIkSystem() = default;
    ~PaddleIkSystem() = default;

    /**
     * @brief initialize of IK system
     * 
     * @param left left attach data
     * @param right right attach data
     * @param ikSkeleton container of bones
     */
    void init(BoneAttachData left, BoneAttachData right, Skeleton* ikSkeleton);

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
    virtual void start() {};

    /**
     * @brief Contain logic of the system called in fixed time steps
     * called in Process() only when AssertEntity() returned true
     * should use class variables to access components
     */
    virtual void fixedUpdate() {};

    /**
     * @brief Contain logic of the system runned once per frame
     * called in Process() only when AssertEntity() returned true
     * should use class variables to access components
     */
    virtual void frameUpdate();

protected:
private:

//TODO documentation
    void processAttach(BoneAttachData data);
    
    Transform* transformPtr;
    Paddle* paddlePtr;

    BoneAttachData attachLeft;
    BoneAttachData attachRight;
    Skeleton* skeleton;
};

#endif /* !CANOEISTIKSYSTEM_HPP_ */
