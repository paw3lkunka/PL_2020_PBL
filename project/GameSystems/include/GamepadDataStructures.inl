#ifndef GAMEPADDATASTRUCTURES_HPP_
#define GAMEPADDATASTRUCTURES_HPP_

#define VERTICAL_SCROLL 0
#define HORIZONTAL_SCROLL 1

/**
 * @brief Structure carring mouse cursor data
 */
struct GamepadButtonData
{
    /**
     * @brief GLFW_JOYSTICK_1 - GLFW_JOYSTICK_16
     */
    int gamepadId;

    /**
     * @brief GLFW_GAMEPAD_BUTTON_[...]
     */
    int buttonId;
};

/**
 * @brief Structure carring mouse scroll data
 */
struct GamepadAxisData
{    
    /**
     * @brief GLFW_JOYSTICK_1 - GLFW_JOYSTICK_16
     */
    int gamepadId;    

    /**
     * @brief GLFW_GAMEPAD_AXIS_[...]
     */
    int axisId;

    //TODO udokumentuj to
    /**
     * 
     * @brief 
     */
    int axisState;
};


#endif /* !GAMEPADDATASTRUCTURES_HPP_ */
