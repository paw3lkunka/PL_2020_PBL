#ifndef _PLAYER_INL
#define _PLAYER_INL

#include "Component.hpp"

/**
 * @brief This structure stores current state of player current state.
 */
struct Kayak : public Component
{
    /**
     * @brief Store number of activated hideouts, can be interpreted as bool.
     */
    int isHidden = 0;
    bool isDetected = false;
};

#endif

