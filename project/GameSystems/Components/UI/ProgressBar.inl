#ifndef PROGRESSBAR_INL_
#define PROGRESSBAR_INL_

#include "Component.hpp"

/**
 * @brief progress bar component class
 */
struct ProgressBar : public Component
{
    ProgressBar() = default;
    ~ProgressBar() = default;

    ///@brief progress in procents (0 - 1)
    float percentage = 1.0f;

    ///@brief last frame percentage
    float lastFramePercentage = 1.0f;
};

#endif /* !PROGRESSBAR_INL_ */
