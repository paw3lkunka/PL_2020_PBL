#include "MessageBus.hpp"

#include "IModule.inl"
#include "Message.inl"

MessageBus::MessageBus(int bufferSize)
: messagesBuffer(bufferSize)
{
    
}

void MessageBus::AddReceiver(IModule* modulePtr)
{
    modulesPointers.push_back(modulePtr);
}

void MessageBus::SendMessage(Message msg)
{
    messagesBuffer.push_back(msg);
}

void MessageBus::Notify()
{
    for(auto ptr : modulesPointers)
    {
        for (auto msg : messagesBuffer)
        {
            ptr->ReceiveMessage(msg);
        }
    }
    messagesBuffer.clear();
}