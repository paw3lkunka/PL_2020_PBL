#ifndef EnemyAttack_inl_sh
#define EnemyAttack_inl_sh

#include "Component.hpp"

/**
 * @brief Store state and parameters of enemy attack. 
 */
struct EnemyAttack : public Component
{
    /**
     * @brief Count time to shoot.
     */
    int attackCounter = 0;

    /**
     * @brief Addaed to attackCounter in each fixed update, if kayak is currently detected.
     */
    int incrementValue = 1;

    /**
     * @brief Enemy shoots, when attackCounter reaches this value.
     */
    int activationValue = __INT_MAX__;

    /**
     * @brief Chance fo successfull hit;
     */
    float successChance = 0.2;

    /**
     * @brief Defines amount of first shoots, that will surely miss.
     */
    float handicap = 0;
};
#endif
