#ifndef _PLAYER_INL
#define _PLAYER_INL

#include "Component.hpp"

/**
 * @brief This structure stores current state of player current state.
 */
struct Player : public Component
{
    bool isHidden = false;
    bool isDetected = false;
};

#endif

