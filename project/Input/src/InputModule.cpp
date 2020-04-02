#include "InputModule.hpp"

#include "GLFW/glfw3.h"

#include "Event.hpp"
#include "Message.inl"
#include "MessageBus.hpp"

MessageBus* InputModule::messageBus;

InputModule::InputModule(MessageBus* messageBus)
{
    this->messageBus = messageBus;
}

void InputModule::initialize(GLFWwindow* window)
{
    glfwSetKeyCallback(window, keyboardCallback);
}

void InputModule::receiveMessage(Message msg)
{
    return;
}

void InputModule::keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Event event;
    switch(action)
    {
        case GLFW_PRESS:
            event = Event::KEY_PRESSED;
            break;
        case GLFW_REPEAT:
            event = Event::KEY_REPEAT;
            break;
        case GLFW_RELEASE:
            event = Event::KEY_RELEASED;
            break;
        default:
            //TODO: exception
            return;
    }
    messageBus->sendMessage(Message(event, key));
}
