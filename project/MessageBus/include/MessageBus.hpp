#ifndef MESSANGEBUS_HPP_
#define MESSANGEBUS_HPP_

#include <vector>

#include "Message.inl"
class IModule;
class ObjectModule;

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
    void addReceiver(IModule* modulePtr);

    /**
     * @brief Send message to MessageBus buffer,
     * receivers will get it, after Notify() invocation.
     * 
     * @param msg - Messange object.
     */
    void sendMessage(Message msg);

    /**
     * @brief Send all buffered messages to receivers,
     * then clean buffer. 
     */
    void notify();

private:
    /**
     * @brief Store received messanges before sending them to receivers
     */
    std::vector<Message> messagesBuffer0;
    
    /**
     * @brief Store received messanges before sending them to receivers
     */
    std::vector<Message> messagesBuffer1;

    /**
     * @brief store pointers to receivers
     */
    std::vector<IModule*> modulesPointers;

    /**
     * @brief pointer to active buffer
     */
    std::vector<Message>* activeBuffer = &messagesBuffer0;
    
    /**
     * @brief pointer to inactive buffer
     */
    std::vector<Message>* inactiveBuffer = &messagesBuffer1;

    ///@brief friendship declaration to clear buffers on scene load
    friend class ObjectModule;
    /**
     * @brief clearing all buffers, on scene load
     */
    void clearBuffers();
};



#endif /* !MESSAGEBUS_HPP_ */
