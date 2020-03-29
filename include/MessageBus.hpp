#ifndef MESSANGEBUS_HPP_
#define MESSANGEBUS_HPP_

#include <vector>

class Message;
class IModule;

/**
 * @brief Bus conecting modules and allowing them to comunicate bay sending and receiving messages 
 */
class MessageBus 
{
    public:
        /**
         * @brief Construct a new Message Bus object
         * 
         * @param bufferSize - Allows to set initial messages buffer size
         */
        MessageBus(int bufferSize = 0);

        /**
         * @brief Assign receiver to MessageBus.
         * 
         * @param modulePtr - Pointer to recever object
         */
        void AddReceiver(IModule* modulePtr);

        /**
         * @brief Send message to MessageBus buffer,
         * receivers will get it, after Notify() invocation.
         * 
         * @param msg - Messange object.
         */
        void SendMessage(Message msg);

        /**
         * @brief Send all buffered messages to receivers,
         * then clean buffer. 
         */
        void Notify();

    private:
        /**
         * @brief Store All received messanges before sending them to receivers
         */
		std::vector<Message> messagesBuffer;

        /**
         * @brief store pointers to receivers
         */
        std::vector<IModule*> modulesPointers;
};



#endif /* !MESSAGEBUS_HPP_ */
