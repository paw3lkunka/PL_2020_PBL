#ifndef ENEMY__
#define ENEMY__

#include "Component.hpp"

/**
 * @brief Store information about enemy.
 */
struct Enemy : public Component
{
    /**
     * @brief Minimum distance between kayak and enemy necessery to detect kayak.
     */
    float sightDistance;

    /**
     * @brief Minimum angle between [enemy,kayak] vector and enemys forward vector necessery to detect kayak.
     * I practice - half of FOV.
     */
    float sightAngle;

    /**
     * @brief Kayak is detected, where detectionCounter reach this value.
     */
    int detectionCounterMaxValue;

    /**
     * @brief Value added to detectionCounter in fixedUpdate, if kayak is seen.
     */
    int detectionPositiveStep = 1;
    
    /**
     * @brief Value substracted to detectionCounter in fixedUpdate, if kayak is not seen.
     */
    int detectionNegativeStep = 1;

    /**
     * @brief Progress of detection.
     */
    int detectionCounter = 0;

    /**
     * @brief Inform, if enemy see kayak.
     * Setted to true, when detectionCounter reach detectionCounterMaxValue.
     * Setted to false, when detectionCounter reach 0.
     */
    bool notified = false;

};

#endif /* !ENEMY__ */
