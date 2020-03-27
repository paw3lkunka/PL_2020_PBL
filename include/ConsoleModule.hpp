#ifndef CONSOLEMODULE_HPP_
#define CONSOLEMODULE_HPP_

#include <iostream>
#include "IModule.inl"

class MessageBus;

/**
 * @brief Write console output depending on received messanges
 */
class ConsoleModule : public IModule {
    public:
        /**
         * @brief Construct a new Console Module object
         * 
         * @param messageBus - Pointer to message bus, used i future to sending events
         */
        ConsoleModule(MessageBus* messageBus);
        
        /**
         * @brief MessageBus pointer, produce console output
         */
        virtual void ReceiveMessage(Message msg);

    private:        
        /**
         * @brief MessageBus pointer, unused as messages destination - temporally unused
         */
        MessageBus* messageBus;
};

#endif /* !CONSOLEMODULE_HPP_ */
