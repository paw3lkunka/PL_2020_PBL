#ifndef PROGRESSBARSYSTEM_HPP_
#define PROGRESSBARSYSTEM_HPP_

struct ProgressBar;
struct UiRenderer;

#include "System.hpp"

/**
 * @brief progress bar system
 */
class ProgressBarSystem : public System
{
public:
    ProgressBarSystem() = default;
    ~ProgressBarSystem() = default;

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
    virtual void fixedUpdate() {};

    /**
     * @brief Contain logic of the system runned once per frame
     * called in Process() only when AssertEntity() returned true
     * should use class variables to access components
     */
    virtual void frameUpdate();

protected:
private:
    ProgressBar* progressPtr;
    UiRenderer* rendererPtr;
};

#endif /* !PROGRESSBARSYSTEM_HPP_ */
