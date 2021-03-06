#ifndef UIBUTTONSYSTEM_HPP_
#define UIBUTTONSYSTEM_HPP_

#include "System.hpp"
#include "IMsgReceiver.inl"
#include "MouseDataStructures.inl"
#include <glm/glm.hpp>

struct Button;
struct RectTransform;
struct UiRenderer;

//TODO documentation
class UiButtonSystem : public IMsgReceiver, public System
{
public:
    UiButtonSystem() : System(false) {}
    ~UiButtonSystem() = default;

    ///@brief inherited from IMsgReceiver
    void receiveMessage(Message msg);

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
    virtual void start() {}
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

protected:
private:
    Button* buttonPtr;
    UiRenderer* rendererPtr;
    RectTransform* rectTransformPtr;
    
    ///@brief data of cursor (to find if cursor is over button)
    CursorData lastCursorData;
    ///@brief mouse button clicked flag
    bool mouseButtonClicked = false;

    ///@brief factor of lerping
    float lerpFactor = 0.05f;
};

#endif /* !UIBUTTONSYSTEM_HPP_ */
