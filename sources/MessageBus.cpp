#include "MessageBus.hpp"

#ifndef NDEBUG
#include <iostream>
#endif

#include "IModule.inl"
#include "Message.inl"

MessageBus::MessageBus(int bufferSize)
{
	messagesBuffer.reserve(bufferSize);
	for (size_t i = 0; i < bufferSize; i++)
	{
		messagesBuffer.push_back(Message());
	}
}

void MessageBus::AddReceiver(IModule* modulePtr)
{
    modulesPointers.push_back(modulePtr);
}

void MessageBus::SendMessage(Message msg)
{
	if (messagesBuffer.size() < messagesBuffer.capacity())
	{
		messagesBuffer.push_back(msg);
	}
	else
	{
#ifndef NDEBUG
		std::cerr << "Message buffer overflow\n";
#endif
		Notify();
	}
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