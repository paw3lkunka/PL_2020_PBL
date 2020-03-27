#include "ConsoleModule.hpp"

#include "MessageBus.hpp"
#include "Message.inl"

ConsoleModule::ConsoleModule(MessageBus* messageBus)
{
    this->messageBus = messageBus;
}       

void ConsoleModule::ReceiveMessage(Message msg)
{
    switch (msg.getEvent())
    {
    case Event::KEY_PRESSED_W:
        std::cout << "console here: Go forwad" << std::endl;
        break;
        
    case Event::KEY_PRESSED_S:
        std::cout << "console here: Go backward" << std::endl;
        break;
    case Event::KEY_PRESSED_A:
        std::cout << "console here: Go left" << std::endl;
        break;
    case Event::KEY_PRESSED_D:
        std::cout << "console here: Go right" << std::endl;
        break;            
    default:
        std::cout << "console here: I don't care!" << std::endl;
        break;
    }
}