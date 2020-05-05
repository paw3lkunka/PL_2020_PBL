#ifndef BONESYSTEM_HPP_
#define BONESYSTEM_HPP_

#include "Skeleton.inl"

#include "System.hpp"

#include <map>

class SkeletonSystem : public System
{
public:
    SkeletonSystem() = default;
    virtual ~SkeletonSystem() = default;

    void process();
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
    
private:
    void processHierarchy(Bone* bone);
    
    std::map<int, glm::mat4> boneTransforms;
    double animationTime;

    Skeleton* skeleton;
};

#endif /* !BONESYSTEM_HPP_ */
