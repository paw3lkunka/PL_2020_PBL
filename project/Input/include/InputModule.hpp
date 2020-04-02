#ifndef INPUTMODULE_HPP_
#define INPUTMODULE_HPP_

#include "IModule.inl"

class Message;
class MessageBus;
class GLFWwindow;

/**
 * @brief Read input from keyboard and send events to message bus.
 * By now, it not respond to any events, but in future it may use events to change internal state  
 */
class InputModule : public IModule
{
    public:
        /**
         * @brief Construct a new Input Module object
         * 
         * @param messageBus - Pointer to message bus, used to sending events
         */
        InputModule(MessageBus* messageBus);

        /**
         * @brief Initialize module by setting GLFW callbacks
         * 
         * @param window GLFW window to track
         */
        void initialize(GLFWwindow* window);

        /**
         * @brief IModule implementation, temporally does nothing
         * 
         * @param msg - Messange to handle
         */
        virtual void receiveMessage(Message msg);
        
    private:
        /**
         * @brief Callback for GLFW keyboard input
         * 
         * @param window GLFW window
         * @param key system-independent key code
         * @param scancode system-dependent key code
         * @param action GLFW_PRESS || GLFW_REPEAT || GLFW_RELEASE
         * @param mods Caps lock and Num Lock state
         */
        static void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        
        /**
         * @brief MessageBus pointer, unused as messages destination
         */
        static MessageBus* messageBus;
};

#endif /* !INPUTMODULE_HPP_ */
