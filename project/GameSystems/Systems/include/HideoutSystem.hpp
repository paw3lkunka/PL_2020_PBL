#ifndef HIDEOUTSYSTEM_HPP_
#define HIDEOUTSYSTEM_HPP_

#include "System.hpp"
#include "IMsgReceiver.inl"

#include <vector>

class Hideout;
class Kayak;
class CollisionData;

class HideoutSystem : public System, public IMsgReceiver
{
    public:
        HideoutSystem() = default;
        virtual ~HideoutSystem() = default;

        virtual bool assertEntity(Entity* entity);
        virtual void receiveMessage(Message msg);
        virtual void fixedUpdate();
        void init();
        void clean();
    protected:
    private:
        Hideout* hideoutPtr;
        Kayak* playerPtr;
        std::vector<Message> messages;
};

#endif /* !HIDEOUTSYSTEM_HPP_ */
