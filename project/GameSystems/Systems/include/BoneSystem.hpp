#ifndef BONESYSTEM_HPP_
#define BONESYSTEM_HPP_

#include "Bone.inl"
#include "Transform.inl"

#include "System.hpp"

class BoneSystem : public System
{
public:
    BoneSystem() = default;

    void process();

protected:
    /**
     * @brief Checks, if given Entity contain all required components
     * implementation should also save pointers to components in class variables 
     * @param entity pointer to asserted entity
     * @returns if entity has all required components
     */
    bool assertEntity(Entity* entity);

    /**
     * @brief Initializes processed entities, runs before first update
     */
    void start();

    /**
     * @brief Contain logic of the system called in fixed time steps
     * called in Process() only when AssertEntity() returned true
     * should use class variables to access components
     */
    void fixedUpdate();

    /**
     * @brief Contain logic of the system runned once per frame
     * called in Process() only when AssertEntity() returned true
     * should use class variables to access components
     */
    void frameUpdate();
    
    virtual ~BoneSystem() = default;
private:
    Bone* bone;
    Transform* transform;
};

#endif /* !BONESYSTEM_HPP_ */
