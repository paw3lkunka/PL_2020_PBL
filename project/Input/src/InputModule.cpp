#include "InputModule.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Event.inl"
#include "Message.inl"
#include "Core.hpp"
#include "MouseDataStructures.inl"
#include "GamepadDataStructures.inl"

unsigned int InputModule::lastCursorMove = 1; // o means, that mouse was moved in this frame
int InputModule::gamepadsEnabled = 0;
GLFWgamepadstate InputModule::gamepads[16] = {};

InputModule::InputModule()
{
}

void InputModule::initialize(GLFWwindow* window)
{
    // Keyboard
    glfwSetKeyCallback(window, keyboardCallback);

    // Mouse
    glfwSetMouseButtonCallback(window, mouseButtonsCallback);
    glfwSetCursorPosCallback(window, cursorCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); 

    // Controllers
    glfwSetJoystickCallback(controllerConnectCallback);
    controllersInitialDetection();
}

void InputModule::receiveMessage(Message msg)
{
    return;
}

void InputModule::captureControllersInput()
{
    GLFWgamepadstate oldState, currentState;

    for( int joy = GLFW_JOYSTICK_1; joy <= GLFW_JOYSTICK_LAST; joy++ )
    {
        if( (gamepadsEnabled >> joy) & 1U )
        {
            oldState = gamepads[joy];
            glfwGetGamepadState(joy, &currentState);
            
            for(int button = 0; button <= GLFW_GAMEPAD_BUTTON_LAST; button++ )
            {
                if( oldState.buttons[button] == GLFW_RELEASE && currentState.buttons[button] == GLFW_PRESS )
                {
                    auto value = GamepadButtonData{joy,button};
                    auto message = Message(Event::GAMEPAD_BUTTON_PRESSED,value);
                    GetCore().getMessageBus().sendMessage(message);
                }
                else if( oldState.buttons[button] == GLFW_PRESS && currentState.buttons[button] == GLFW_RELEASE )
                {
                    auto value = GamepadButtonData{joy,button};
                    auto message = Message(Event::GAMEPAD_BUTTON_RELEASED,value);
                    GetCore().getMessageBus().sendMessage(message);
                }
            }

            for (int axis = 0; axis <= GLFW_GAMEPAD_AXIS_LAST; axis++)
            {
                if( oldState.axes[axis] != currentState.axes[axis] )
                {
                    auto value = GamepadAxisData{joy,axis,currentState.axes[axis]};
                    auto message = Message(Event::GAMEPAD_AXIS_CHANGED,value);
                    GetCore().getMessageBus().sendMessage(message);
                }
            }            

            gamepads[joy] = currentState;
        }
    }
}

void InputModule::clearFlags()
{
    if(lastCursorMove++ == 1)
    {
        GetCore().getMessageBus().sendMessage(Message(Event::MOUSE_CURSOR_STOPPED));
    }

}

void InputModule::controllersInitialDetection()
{
    for( int joy = GLFW_JOYSTICK_1; joy <= GLFW_JOYSTICK_LAST; joy++ )
    {
        if( glfwJoystickPresent(joy) && glfwJoystickIsGamepad(joy) )
        {   
            gamepadsEnabled |= (1 << joy);
            auto message = Message(Event::GAMEPAD_CONNECTED,joy);
            GetCore().getMessageBus().sendMessage(message);
        }
    }
}

void InputModule::keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // ! ----- Quick fix for function buttons crashing the application -----
    if (action == GLFW_KEY_UNKNOWN || key == GLFW_KEY_UNKNOWN)
    {
        return;
    }

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
    }
    GetCore().getMessageBus().sendMessage(Message(event, key));
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
    }
    GetCore().getMessageBus().sendMessage(Message(event, button));
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
    
    GetCore().getMessageBus().sendMessage(Message(Event::MOUSE_CURSOR_MOVED, data));

    xOld = xPos;
    yOld = yPos;
    lastCursorMove = 0;
}

void InputModule::scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
    if(xOffset)
    {
        auto message = Message(Event::MOUSE_SCROLL_MOVED, ScrollData{xOffset, VERTICAL_SCROLL});
        GetCore().getMessageBus().sendMessage(message);
    }

    if(yOffset)
    {
        auto message = Message(Event::MOUSE_SCROLL_MOVED, ScrollData{yOffset, HORIZONTAL_SCROLL});
        GetCore().getMessageBus().sendMessage(message);
    }
}

void InputModule::controllerConnectCallback(int id, int glfwEvent)
{
    Event event;
    switch (glfwEvent)
    {
    case GLFW_CONNECTED:
        if(glfwJoystickIsGamepad(id))
        {
            gamepads[id] = GLFWgamepadstate();
            event = Event::GAMEPAD_CONNECTED;
            gamepadsEnabled |= (1 << id);
            GetCore().getMessageBus().sendMessage(Message(event,id));
        }        
        break;

    case GLFW_DISCONNECTED:    
        if( (gamepadsEnabled >> id) & 1U )
        {
            event = Event::GAMEPAD_DISCONNECTED;
            gamepadsEnabled &= ~(1 << id);   
            GetCore().getMessageBus().sendMessage(Message(event,id));
        }     
        break;
    }
}