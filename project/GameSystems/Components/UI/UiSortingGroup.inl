#ifndef UISORTINGGROUP_INL_
#define UISORTINGGROUP_INL_

#include "Component.hpp"

/**
 * @brief transparency sorting group for ui components
 */
struct UiSortingGroup : public Component
{
    UiSortingGroup() = default;
    ~UiSortingGroup() = default;
    float groupTransparency = 0.0f;
    float lastFrameTransparency = 0.0f;
    ///@brief time after group disappears
    float disappearanceTime = 0.0f;
    ///@brief time counter
    float startTime = 0.0f;
    ///@brief factor of mix method
    float disappearanceFactor = 0.15f;
};

#endif /* !UISORTINGGROUP_INL_ */
