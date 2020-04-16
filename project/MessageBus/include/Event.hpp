#ifndef EVENT_HPP_
#define EVENT_HPP_

/**
 * @brief Enum containing all possible event types * 
 */
enum class Event : unsigned int
{
    // Most probably error, shouldn't be sent intentionally, DATA: none.
    UNKNOWN_EVENT = 0,

#pragma region DEBUG
    // Debug log, info priority, DATA: Pointer to const char* log.
    DEBUG_INFO_LOG,
    // Debug log, warning priority, DATA: Pointer to const char* log.
    DEBUG_WARNING_LOG,
    // Debug log, error priority, DATA: Pointer to const char* log.
    DEBUG_ERROR_LOG,
#pragma endregion

#pragma region INPUT
    // The key was pressed, DATA: int - GLFW keycode.
    KEY_PRESSED,
    // The key was released, DATA: int - GLFW keycode.
    KEY_RELEASED,
    // The key was held down until it repeated int - GLFW keycode.
    KEY_REPEAT,
    // The mouse button was pressed, DATA: int - GLFW mouse button.
    MOUSE_BUTTON_PRESSED,
    // The mouse button was released, DATA: int - GLFW mouse button.
    MOUSE_BUTTON_RELEASED,
    // The mouse cursor was moved, DATA: CursorData {double xPos, double yPos, double xDelta, double yDelta}
    MOUSE_CURSOR_MOVED,
    // The mouse cursor was stopped, DATA: none
    MOUSE_CURSOR_STOPPED,
    // The mouse scroll was moved, DATA: ScrollData {double offset, int axis VERTICAL_SCROLL || HORIZONTAL_SCROLL}
    MOUSE_SCROLL_MOVED,
    // The gamepad was connected, DATA: int - controller id: GLFW joystick number.
    GAMEPAD_CONNECTED,
    // The gamepad was connected, DATA: int - controller id: GLFW joystick number.
    GAMEPAD_DISCONNECTED,
    // The gamepad button was pressed, DATA: GamepadButtonData {int gamepadId - GLFW joystick, int buttonId - GLFW gamepad button}.
    GAMEPAD_BUTTON_PRESSED,
    // The gamepad button was released, DATA: GamepadButtonData {int gamepadId - GLFW joystick, int buttonId - GLFW gamepad button}.
    GAMEPAD_BUTTON_RELEASED,
    // The gamepad axis pressure amount was changed, DATA: GamepadAxisData {int gamepadId - GLFW joystick, int axisId - GLFW gamepad button, float amount}.
    GAMEPAD_AXIS_CHANGED,
#pragma endregion
    
    // used to define ranges of values
#pragma region MARKERS
    DEBUG_FIRST = DEBUG_INFO_LOG,
    DEBUG_LAST = DEBUG_ERROR_LOG,
    INPUT_FIRST = KEY_PRESSED,
    INPUT_LAST = GAMEPAD_AXIS_CHANGED,
#pragma endregion

};

#endif /* !EVENT_HPP_ */
