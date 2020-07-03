#ifndef TUTORIALCONTROLLERSYSTEM_HPP_
#define TUTORIALCONTROLLERSYSTEM_HPP_

#include "System.hpp"

struct Tutorial;
struct RectTransform;
struct UiRenderer;
class Texture;

class TutorialControllerSystem : public System
{
public:
    TutorialControllerSystem() : System(false) {};
    ~TutorialControllerSystem() = default;

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
    RectTransform* rectTransformPtr = nullptr;
    Tutorial* tutorialPtr = nullptr;
    UiRenderer* rendererPtr = nullptr;

    Texture* animationFrame[13] = {nullptr};

    int boardIndex = 0;
};

#endif /* !TUTORIALCONTROLLERSYSTEM_HPP_ */
