#include "PhysicalBasedInputSystem.hpp"
#include "Message.inl"
#include "Entity.hpp"
#include "Components.inc"
#include <exception>

void PhysicalBasedInputSystem::clearKeysets()
{
    pressed.clear();
}

void PhysicalBasedInputSystem::receiveMessage(Message msg)
{
    if (msg.getEvent() == Event::KEY_PRESSED)
    {
        pressed.insert(msg.getValue<int>());
        held.insert(msg.getValue<int>());
    }
    else if (msg.getEvent() == Event::KEY_RELEASED)
    {
        auto iterator = held.find(msg.getValue<int>());
        held.erase(iterator);
    }
}

bool PhysicalBasedInputSystem::assertEntity(Entity* entity)
{
    keymapPtr = entity->getComponentPtr<PhysicalInputKeymap>();
    rigidbodyPtr = entity->getComponentPtr<Rigidbody>();

    return keymapPtr != nullptr && rigidbodyPtr != nullptr;
}

void PhysicalBasedInputSystem::fixedUpdate()
{
    for (int keycode: pressed)
    {
        try
        {
            rigidbodyPtr->impulses.push_back(keymapPtr->single.at(keycode));
            std::cout<<"impulse #1"<<std::endl;
        }
        catch(std::out_of_range){}
    }

    for (int keycode: held)
    {
        try
        {
            rigidbodyPtr->impulses.push_back(keymapPtr->continuous.at(keycode));
            std::cout<<"impulse #3"<<std::endl;
        }
        catch(std::out_of_range){}
    }
}