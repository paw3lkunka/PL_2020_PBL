#ifndef BONESSYSTEM_HPP_
#define BONESSYSTEM_HPP_

#include "System.hpp"

class BonesSystem : public System
{
public:
    BonesSystem() = default;

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
    
    virtual ~BonesSystem() = default;
private:

};

#endif /* !BONESSYSTEM_HPP_ */
