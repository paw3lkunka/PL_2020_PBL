#ifndef ENEMY_DATA_STRUCTURES_INL
#define ENEMY_DATA_STRUCTURES_INL

class Enemy;

/**
 * @brief Stores information about performed attack. 
 */
struct AttackData
{
    /**
     * @brief Pointer to attacking enemy. 
     */
    Enemy* enemyPtr;
    
    /**
     * @brief Attack direction. 
     */
    glm::vec3 direction;

    /**
     * @brief if attack was successful. 
     */
    bool success;
};

#endif