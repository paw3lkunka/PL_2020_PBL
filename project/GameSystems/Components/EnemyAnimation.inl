#ifndef EnemyAnimation_inl_sh
#define EnemyAnimation_inl_sh

#include "Component.hpp"

//TODO documentation
struct EnemyAnimation : public Component
{
    /**
     * @brief Deterines speed of 'look at' animation
     *  < 0  - undefined
     *   0   - no move
     * [0,1] - animation
     *   1   - instant look at
     *  > 1  - undefined
     */
    float lerpParameter = 1;
};
#endif
