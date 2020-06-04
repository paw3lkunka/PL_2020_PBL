/*
** EPITECH PROJECT, 2020
** pbl
** File description:
** Enemy
*/

#ifndef ENEMY__
#define ENEMY__

#include "Component.hpp"

//TODO documentation
struct Enemy : public Component
{
    //TODO documentation
    float sightDistance;

    //TODO documentation
    int detectionCounterMaxValue;

    //TODO documentation
    int detectionPositiveStep = 1;
    
    //TODO documentation
    int detectionNegativeStep = 1;

    //TODO documentation
    int detectionCounter = 0;

    //TODO documentation
    bool notified = false;

};

#endif /* !ENEMY__ */
