#ifndef PHYSICALINPUTKEYMAP_INL
#define PHYSICALINPUTKEYMAP_INL

#include "Component.inl"
#include <unordered_map>
#include <unordered_set>
#include <glm/glm.hpp>
#include "PhisicStructures.inl"

/**
 * @brief keymap used by PhysicalBasedInputSystem
 * 
 */
struct PhysicalInputKeymap : public Component
{
    //TODO uncomment, when mouse and gamepad will be necessary
    /*
    static constexpr int KEYBOARD   = 0b0000_0001;
    static constexpr int MOUSE      = 0b0000_0010;
    static constexpr int GAMEPAD    = 0b0000_0100;

    /**
     * @brief stores informations about used controllers,
     * turning on these bits allows PhysicalBasedInputSystem to read:
     * 0 - keyboard
     * 1 - mouse
     * 2 - gamepad
     */
    //int controllersMask = KEYBOARD;

    /**
     * @brief strores keycodes and Impulses apliled single time on key press 
     */
    std::unordered_map<int,Impulse> single;

    /**
     * @brief strores keycodes and Forces acting as long, as appropriate key is down
     */
    std::unordered_map<int,Impulse> continuous;
    
};

#endif