#ifndef BUTTON_INL_
#define BUTTON_INL_

#include "Component.hpp"
#include <glm/vec4.hpp>
#include "Event.inl"

struct Button : public Component
{
    Button() = default;
    virtual ~Button() = default;

    bool isActive = true;
    glm::vec4 baseColor;
    glm::vec4 highlightedColor;
    glm::vec4 inactiveColor;
    glm::vec4 onClickColor;
    Event onClickEvent;
};

#endif /* !BUTTON_INL_ */
