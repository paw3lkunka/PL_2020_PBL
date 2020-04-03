#include "ConsoleModule.hpp"

#include "MessageBus.hpp"
#include "Message.inl"
#include "MouseDataStructures.hpp"

ConsoleModule::ConsoleModule(MessageBus* messageBus)
{
    this->messageBus = messageBus;
}       

void ConsoleModule::receiveMessage(Message msg)
{
    switch (msg.getEvent())
    {
    case Event::KEY_PRESSED:
        std::cout << "console here: KEY_PRESSED. code: " << msg.getValue<int>() << std::endl;
        break;
        
    case Event::KEY_RELEASED:
        std::cout << "console here: KEY_RELEASED. code: " << msg.getValue<int>() << std::endl;
        break;
    case Event::KEY_REPEAT:
        std::cout << "console here: KEY_REPEAT. code: " << msg.getValue<int>() << std::endl;
        break;
    case Event::MOUSE_BUTTON_PRESSED:
        std::cout << "console here: MOUSE_BUTTON_PRESSED. code: " << msg.getValue<int>() << std::endl;
        break;
    case Event::MOUSE_BUTTON_RELEASED:
        std::cout << "console here: MOUSE_BUTTON_RELEASED. code: " << msg.getValue<int>() << std::endl;
        break;
    case Event::MOUSE_CURSOR_MOVED:
    {
        CursorData cData = msg.getValue<CursorData>();
        std::cout << "console here: MOUSE_CURSOR_MOVED. pos: " << cData.xPos << ", " << cData.yPos << ", delta: " << cData.xDelta << ", " << cData.yDelta << std::endl;
    }
        break;
    case Event::MOUSE_CURSOR_STOPPED:
        std::cout << "console here: MOUSE_CURSOR_STOPPED." << std::endl;
        break;
    case Event::MOUSE_SCROLL_MOVED:
    {
        ScrollData sData = msg.getValue<ScrollData>();
        std::cout << "console here: MOUSE_SCROLL_MOVED. " << (sData.axis ? "Vertical" : "Horizontal") << " scroll: " << sData.offset << std::endl;
    }
        break;
    }
}