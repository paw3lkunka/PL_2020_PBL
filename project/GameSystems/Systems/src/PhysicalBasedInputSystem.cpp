#include "PhysicalBasedInputSystem.hpp"
#include "Message.inl"
#include "Entity.hpp"
#include "Components.inc"
#include <exception>

void PhysicalBasedInputSystem::clearKeysets()
{
    pressed.clear();
    repeated.clear();
}

void PhysicalBasedInputSystem::receiveMessage(Message msg)
{
    if (msg.getEvent() == Event::KEY_PRESSED)
    {
        pressed.insert(msg.getValue<int>());
    }
    else if (msg.getEvent() == Event::KEY_REPEAT)
    {
        repeated.insert(msg.getValue<int>());
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
            rigidbodyPtr->impulses.push_back(keymapPtr->continous.at(keycode));
        }
        catch(std::out_of_range){}
    }

    for (int keycode: repeated)
    {
        try
        {
            rigidbodyPtr->impulses.push_back(keymapPtr->continous.at(keycode));
        }
        catch(std::out_of_range){}
    }
}