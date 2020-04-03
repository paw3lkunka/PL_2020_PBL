#ifndef INPUTMODULE_HPP_
#define INPUTMODULE_HPP_

#include "IModule.inl"

class Message;
class MessageBus;
class GLFWwindow;

/**
 * @brief Read input from keyboard and send events to message bus.
 * By now, it not respond to any events, but in future it may use events to change internal state.
 */
class InputModule : public IModule
{
    public:
        /**
         * @brief Construct a new Input Module object.
         * To work properly require to call initialize() before use
         * and clearFlags() in the end of each frame.
         * MessageBus pointer is static, so it's highly recommended not to create more than one instance.
         * @param messageBus - Pointer to message bus, used to sending events
         */
        InputModule(MessageBus* messageBus);

        /**
         * @brief Initialize module by setting GLFW callbacks.
         * 
         * @param window GLFW window to track.
         */
        void initialize(GLFWwindow* window);

        /**
         * @brief IModule implementation, temporally does nothing.
         * 
         * @param msg - Messange to handle.
         */
        virtual void receiveMessage(Message msg);

        /**
         * @brief Clear internal flags, should be called in end of each frame .
         */
        void clearFlags();
    private:

        /**
         * @brief Number of frames from last cursor move, incremented by clearFlags(), resetted by cursorCallback().
         */
        static unsigned int lastCursorMove;
        
        /**
         * @brief MessageBus pointer, unused as messages destination.
         */
        static MessageBus* messageBus;

        /**
         * @brief Callback for GLFW keyboard input.
         * 
         * @param window GLFW window.
         * @param key system-independent key code.
         * @param scancode system-dependent key code.
         * @param action GLFW_PRESS || GLFW_REPEAT || GLFW_RELEASE.
         * @param mods Caps lock and Num Lock state.
         */
        static void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        
        /**
         * @brief Callback for GLFW mouse buttons input.
         * 
         * @param window GLFW window.
         * @param button key code.
         * @param action GLFW_PRESS || GLFW_RELEASE.
         * @param mods ???
         */
        static void mouseButtonsCallback(GLFWwindow* window, int button, int action, int mods);

        /**
         * @brief Callback for GLFW mouse cursor input.
         * 
         * @param window GLFW window.
         * @param xPos horizontal coordinate.
         * @param yPos vertical coordinate.
         */
        static void cursorCallback(GLFWwindow* window, double xPos, double yPos);

        /**
         * @brief Callback for GLFW moouse scroll or touchpad scroll gesture.
         * 
         * @param window GLFW window.
         * @param xOffset offset of vertical scroll.
         * @param yOffset offset of horizontal scroll.
         */
        static void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);        
};

#endif /* !INPUTMODULE_HPP_ */
