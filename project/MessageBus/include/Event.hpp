#ifndef EVENT_HPP_
#define EVENT_HPP_

/**
 * @brief Enum containing all possible event types * 
 */
enum class Event : unsigned int
{
    UNKNOWN_EVENT = 0,
    // int - GLFW keycode, The key or mouse button was released.
    KEY_PRESSED,
    // int - GLFW keycode, The key or mouse button was pressed.
    KEY_RELEASED,
    // int - GLFW keycode,The key was held down until it repeated.
    KEY_REPEAT

};

#endif /* !EVENT_HPP_ */
