#ifndef INPUTMODULE_HPP_
#define INPUTMODULE_HPP_

#include "Message.inl"
#include "IModule.inl"
#include "MessageBus.hpp"

#ifdef __linux__ 
    // it needs some compiler option to make getch() work
    //#include <curses.h>
    #include <iostream>
#elif _WIN32
    #include <conio.h>
#endif

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
