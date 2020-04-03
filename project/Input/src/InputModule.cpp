#include "InputModule.hpp"

#include "GLFW/glfw3.h"

#include "Event.hpp"
#include "Message.inl"
#include "MessageBus.hpp"
#include "MouseDataStructures.hpp"

MessageBus* InputModule::messageBus = nullptr;
unsigned int InputModule::lastCursorMove = 1; // o means, that mouse was moved in this frame

InputModule::InputModule(MessageBus* messageBus)
{
    this->messageBus = messageBus;
}

void InputModule::initialize(GLFWwindow* window)
{
    glfwSetKeyCallback(window, keyboardCallback);
    glfwSetMouseButtonCallback(window, mouseButtonsCallback);
    glfwSetCursorPosCallback(window, cursorCallback);
    glfwSetScrollCallback(window, scrollCallback);
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

void InputModule::clearFlags()
{
    if(lastCursorMove++ == 1)
    {
        messageBus->sendMessage(Message(Event::MOUSE_CURSOR_STOPPED));
    }

}

void InputModule::mouseButtonsCallback(GLFWwindow* window, int button, int action, int mods)
{
    Event event;
    switch(action)
    {
        case GLFW_PRESS:
            event = Event::MOUSE_BUTTON_PRESSED;
            break;           
        case GLFW_RELEASE:
            event = Event::MOUSE_BUTTON_RELEASED;
            break;
        default:
            //TODO: exception
            return;
    }
    messageBus->sendMessage(Message(event, button));
}

void InputModule::cursorCallback(GLFWwindow* window, double xPos, double yPos)
{
    static int xOld = 0;
    static int yOld = 0;

    auto data = CursorData
    {
        //conversion to float limits structure size
        static_cast<float>(xPos),
        static_cast<float>(yPos),        
        static_cast<float>(xPos - xOld),
        static_cast<float>(yPos - yOld)
    };
    
    if(lastCursorMove > 1)
    {
        data.xDelta = 0;
        data.yDelta = 0;
    }
    
    messageBus->sendMessage(Message(Event::MOUSE_CURSOR_MOVED, data));

    xOld = xPos;
    yOld = yPos;
    lastCursorMove = 0;
}

void InputModule::scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
    if(xOffset)
    {
        auto message = Message(Event::MOUSE_SCROLL_MOVED, ScrollData{xOffset, VERTICAL_SCROLL});
        messageBus->sendMessage(message);
    }

    if(yOffset)
    {
        auto message = Message(Event::MOUSE_SCROLL_MOVED, ScrollData{yOffset, HORIZONTAL_SCROLL});
        messageBus->sendMessage(message);
    }
}
