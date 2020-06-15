#ifndef EnemyAnimation_inl_sh
#define EnemyAnimation_inl_sh

#include "Component.hpp"

/**
 * @brief Store all data defining 'look at' animation.
 */
struct EnemyAnimation : public Component
{
    /**
     * @brief Deterines speed of 'look at' animation.
     *  < 0  - undefined
     *   0   - no move
     * [0,1] - animation
     *   1   - instant look at
     *  > 1  - undefined
     */
    float lerpParameter = 1;

    /**
     * @brief Time of showing pullet trail.
     * NOTE: Trails show always at least 1 frame.
     */
    float shootTrailTime = 0;
};
#endif
