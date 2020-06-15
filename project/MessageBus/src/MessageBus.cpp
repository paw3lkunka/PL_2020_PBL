#include "MessageBus.hpp"

#include <iostream>

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
		std::cout << "Message buffer overflow\n";
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

void MessageBus::clearBuffers()
{
	activeBuffer->clear();
	inactiveBuffer->clear();
}