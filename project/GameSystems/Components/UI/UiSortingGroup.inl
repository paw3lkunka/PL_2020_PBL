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
    float groupTransparency = 0.8f;
    float lastFrameTransparency = 0.8f;
};

#endif /* !UISORTINGGROUP_INL_ */