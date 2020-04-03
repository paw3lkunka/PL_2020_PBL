#ifndef EVENT_HPP_
#define EVENT_HPP_

/**
 * @brief Enum containing all possible event types * 
 */
enum class Event : unsigned int
{
    // Most probably error, shouldn't be sended intentionally, DATA: none
    UNKNOWN_EVENT = 0,
    // The key was pressed, DATA: int - GLFW keycode.
    KEY_PRESSED,
    // The key was released, DATA: int - GLFW keycode.
    KEY_RELEASED,
    // The key was held down until it repeated int - GLFW keycode.
    KEY_REPEAT,
    // The mouse button  was pressed, DATA: int - GLFW mouse button.
    MOUSE_BUTTON_PRESSED,
    // The mouse button  was released, DATA: int - GLFW mouse button.
    MOUSE_BUTTON_RELEASED,
    // The mouse cursor was moved, DATA: CursorData {double xPos, double yPos, double xDelta, double yDelta}
    MOUSE_CURSOR_MOVED,
    // The mouse cursor was stopped, DATA: none
    MOUSE_CURSOR_STOPPED,
    // The mouse scroll was moved, DATA: ScrollData {double offset, int axis VERTICAL_SCROLL || HORIZONTAL_SCROLL}
    MOUSE_SCROLL_MOVED,

};

#endif /* !EVENT_HPP_ */
