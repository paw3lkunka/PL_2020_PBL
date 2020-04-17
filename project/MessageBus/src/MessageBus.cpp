#include "MessageBus.hpp"

#ifndef NDEBUG
#include <iostream>
#endif

#include "IModule.inl"
#include "Message.inl"

MessageBus::MessageBus(int bufferSize)
{
	messagesBuffer0.reserve(bufferSize);
	messagesBuffer1.reserve(bufferSize);
}

void MessageBus::addReceiver(IModule* modulePtr)
{
    modulesPointers.push_back(modulePtr);
}

void MessageBus::sendMessage(Message msg)
{
	if (activeBuffer->size() < activeBuffer->capacity())
	{
		activeBuffer->push_back(msg);
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
	auto tmp = activeBuffer;
	activeBuffer = inactiveBuffer;
	inactiveBuffer = tmp;	

    for(auto ptr : modulesPointers)
    {
        for (auto msg : *inactiveBuffer)
        {
            ptr->receiveMessage(msg);
        }
    }
    inactiveBuffer->clear();
}
