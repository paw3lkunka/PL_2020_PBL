#include "ConsoleModule.hpp"

#include "MessageBus.hpp"
#include "Message.inl"

ConsoleModule::ConsoleModule(MessageBus* messageBus)
{
    this->messageBus = messageBus;
}       

void ConsoleModule::receiveMessage(Message msg)
{
    switch (msg.getEvent())
    {
    case Event::KEY_PRESSED:
        std::cout << "console here: KEY_PRESSED " << msg.getValue<int>() << std::endl;
        break;
        
    case Event::KEY_RELEASED:
        std::cout << "console here: KEY_RELEASED " << msg.getValue<int>() << std::endl;
        break;
    case Event::KEY_REPEAT:
        std::cout << "console here: KEY_REPEAT " << msg.getValue<int>() << std::endl;
        break;
    }
}