#include "MessageBus.hpp"

#ifndef NDEBUG
#include <iostream>
#endif

#include "IModule.inl"
#include "Message.inl"

MessageBus::MessageBus(int bufferSize)
{
	messagesBuffer.reserve(bufferSize);
}

void MessageBus::addReceiver(IModule* modulePtr)
{
    modulesPointers.push_back(modulePtr);
}

void MessageBus::sendMessage(Message msg)
{
	if (messagesBuffer.size() < messagesBuffer.capacity())
	{
		messagesBuffer.push_back(msg);
	}
	else
	{		
//TODO discuss
#ifndef NDEBUG
		std::cerr << "Message buffer overflow\n";
#endif
		notify();
	}
}

void MessageBus::notify()
{
    for(auto ptr : modulesPointers)
    {
        for (auto msg : messagesBuffer)
        {
            ptr->receiveMessage(msg);
        }
    }
    messagesBuffer.clear();
}
