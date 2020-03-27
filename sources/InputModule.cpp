#include "InputModule.hpp"

InputModule::InputModule(MessageBus* messageBus)
{
    this->messageBus = messageBus;
}

void InputModule::MockConsoleInput()
{
    Message msg;
    switch (getch())
    {
    case 'w':
    case 'W':
        msg = Message(Event::KEY_PRESSED_W);
        break;
        
    case 's':
    case 'S':
        msg = Message(Event::KEY_PRESSED_S);
        break;
        
    case 'a':
    case 'A':
        msg = Message(Event::KEY_PRESSED_A);
        break;
        
    case 'd':
    case 'D':
        msg = Message(Event::KEY_PRESSED_D);
        break;
    }
    if (msg.getEvent() != Event::UNKNOWN_EVENT)
    {
        messageBus->SendMessage(msg);
    }
}        

void InputModule::ReceiveMessage(Message msg)
{
    return;
}