#ifndef BUTTON_INL_
#define BUTTON_INL_

#include <glm/vec4.hpp>
#include <vector>
#include "Component.hpp"
#include "Message.inl"

struct Button : public Component
{
    Button() = default;
    virtual ~Button() = default;

    ///@brief isActive flag - if button can trigger an event
    bool isActive = true;
    ///@brief isClicked flag - checks if button is clicked by user
    bool isClicked = false;
    ///@brief base color of the active button
    glm::vec4 baseColor = {0, 0, 0, 1};
    ///@brief color of inactive button
    glm::vec4 inactiveColor = {0, 0, 0, 1};
    ///@brief color when button is highlighted by mouse pointer
    glm::vec4 highlightedColor = {0, 0, 0, 1};
    ///@brief color when button is clicked
    glm::vec4 onClickColor = {0, 0, 0, 1};
    ///@brief button color in last frame
    glm::vec4 lastFrameColor;
    ///@brief events - messages - to send when button is clicked
    std::vector<Message> onClickEvents;
    //!IMPORTANT it's message, because we need also value of event if it's eg. AUDIO_SOURCE_PLAY
};

#endif /* !BUTTON_INL_ */
