#include "GamePlayModule.hpp"
#include "Message.inl"
#include "Core.hpp"

void GamePlayModule::receiveMessage(Message msg)
{
    switch(msg.getEvent())
    {
        case Event::EXIT_GAME:
            Core::instance->close();
        break;
        case Event::KEY_PRESSED:
            if(msg.getValue<int>() == GLFW_KEY_ESCAPE)
            {
                Core::instance->messageBus.sendMessage(Message(Event::EXIT_GAME));
            }
            if(msg.getValue<int>() == GLFW_KEY_P)
            {
                Core::instance->messageBus.sendMessage(Message(Event::PAUSE_GAME));
            }
        break;
        case Event::PAUSE_GAME:
            Core::instance->gamePaused = !Core::instance->gamePaused;
            Core::instance->messageBus.sendMessage(Message(Event::AUDIO_SOURCE_PAUSE_ALL_PLAYING));
        break;
        
    }
}
