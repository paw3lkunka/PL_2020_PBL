#ifndef PHYSICALBASEDINPUTSYSTEM_HPP_
#define PHYSICALBASEDINPUTSYSTEM_HPP_

#include "System.hpp"
#include "IMsgReceiver.inl"

#include <unordered_set>

class Rigidbody;
class PhysicalInputKeymap;

    //TODO DOCUMENTATION
class PhysicalBasedInputSystem : public System, public IMsgReceiver
{
    public:
        PhysicalBasedInputSystem() = default;
        virtual ~PhysicalBasedInputSystem() = default;

        /**
         * @brief should pe called after every cleanup
         */
        void clearKeysets();
    protected:
        virtual void receiveMessage(Message msg);
        virtual bool assertEntity(Entity* entity);
        virtual void fixedUpdate();

    private:
        PhysicalInputKeymap* keymapPtr;
        Rigidbody* rigidbodyPtr;

        std::unordered_set<int> pressed;
        std::unordered_set<int> repeated;
};

#endif /* !PHYSICALBASEDINPUTSYSTEM_HPP_ */
