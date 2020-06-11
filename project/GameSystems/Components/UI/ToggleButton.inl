#ifndef SWITCH_INL_
#define SWITCH_INL_

#include <glm/vec4.hpp>
#include <vector>

#include "Component.hpp"
#include "Message.inl"

/**
 * @brief toggle button component - two position switching device
 * saves its state and have separate events for On and Off events
 */
struct ToggleButton : public Component
{
    ToggleButton() = default;
    ~ToggleButton()
    {
        onActivateEvents.clear();
        onDeactivateEvents.clear();
    }

    ///@brief determines if button can be clicked (used)
    bool isActive = true;
    ///@brief isClicked flag - checks if button is clicked by user
    bool isClicked = false;
    
    ///@brief actual state of toggle button, false = OFF, true = ON
    bool state = false; 
    ///@brief inactive switch color
    glm::vec4 inactiveColor = {0.0f, 0.0f, 0.0f, 1.0f};

    //* colors for ON state
    glm::vec4 baseColorOn = {0.0f, 0.0f, 0.0f, 1.0f};
    glm::vec4 highlightedColorOn = {0.0f, 0.0f, 0.0f, 1.0f};
    glm::vec4 onClickColorOn = {0.0f, 0.0f, 0.0f, 1.0f};

    //* colors for OFF state
    glm::vec4 baseColorOff = {0.0f, 0.0f, 0.0f, 1.0f};
    glm::vec4 highlightedColorOff = {0.0f, 0.0f, 0.0f, 1.0f};
    glm::vec4 onClickColorOff = {0.0f, 0.0f, 0.0f, 1.0f};

    ///@brief button color in last frame - for interpolation
    glm::vec4 lastFrameColor;

    ///@brief events - messages - to send when button state changes from OFF to ON - activation
    std::vector<Message> onActivateEvents;

    ///@brief events - messages - to send when button state changes from ON to OFF - deactivation
    std::vector<Message> onDeactivateEvents;
};

#endif /* !SWITCH_INL_ */
