#ifndef INPUTMODULE_HPP_
#define INPUTMODULE_HPP_

#include <conio.h>
#include "Message.inl"
#include "IModule.inl"
#include "MessageBus.hpp"

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
         * @brief Temporary keyboard handler basing on getch() from conio.h
         * in future in will handle input devices from GLAD
         */
        void MockConsoleInput();

        /**
         * @brief IModule implementation, temporally does nothing
         * 
         * @param msg - Messange to handle
         */
        virtual void ReceiveMessage(Message msg);
        
    private:
        /**
         * @brief MessageBus pointer, unused as messages destination
         */
        MessageBus* messageBus;
};

#endif /* !INPUTMODULE_HPP_ */
