#ifndef CONSOLEMODULE_HPP_
#define CONSOLEMODULE_HPP_

#include "IModule.inl"

class MessageBus;

/**
 * @brief Write console output depending on received messanges
 */
class ConsoleModule : public IModule {
    public:
        /**
         * @brief Construct a new Console Module object
         */
        ConsoleModule();
        
        /**
         * @brief MessageBus pointer, produce console output
         */
        virtual void receiveMessage(Message msg);

    private:

};

#endif /* !CONSOLEMODULE_HPP_ */
