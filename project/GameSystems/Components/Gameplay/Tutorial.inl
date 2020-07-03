#ifndef TUTORIAL_INL_
#define TUTORIAL_INL_

#include "Component.hpp"

/**
 * @brief Tutorial component
 */
struct Tutorial : public Component
{
    Tutorial() = default;
    ~Tutorial() = default;

    ///@brief animation time (in seconds) of one board
    float animationTime = 0.5f;

    ///@brief time (in seconds) of one tutorial board
    float timeOfBoard = 0.7f;

    ///@brief time (in seconds) of scene
    float sceneTime = 0.0f;

    ///@brief frame time of board
    float frameTime = 0.0f;

    ///@brief is tutorial shown
    bool shown = true;
};

#endif /* !TUTORIAL_INL_ */
