#ifndef PHYSICALBASEDINPUTSYSTEM_HPP_
#define PHYSICALBASEDINPUTSYSTEM_HPP_

#include "System.hpp"
#include "IMsgReceiver.inl"

#include <unordered_set>

class Rigidbody;
class PhysicalInputKeymap;

/**
 * @brief System responsible for applying physical impulses, as reaction to input events.
 */
class PhysicalBasedInputSystem : public System, public IMsgReceiver
{
    public:
        PhysicalBasedInputSystem() = default;
        virtual ~PhysicalBasedInputSystem() = default;

        /**
         * @brief should be called after every update
         */
        void clearKeysets();
    protected:
        virtual void receiveMessage(Message msg);
        virtual bool assertEntity(Entity* entity);
        virtual void fixedUpdate();

    private:
        PhysicalInputKeymap* keymapPtr;
        Rigidbody* rigidbodyPtr;

        /// @brief store keys pressed exactly in last frame
        std::unordered_set<int> pressed;
        /// @brief store keys pressed some frames before, and still held down
        std::unordered_set<int> held;
};

#endif /* !PHYSICALBASEDINPUTSYSTEM_HPP_ */
