#ifndef HIDEOUTSYSTEM_HPP_
#define HIDEOUTSYSTEM_HPP_

#include "System.hpp"
#include "IMsgReceiver.inl"

#include <vector>

class Hideout;
class Player;
class CollisionData;

class HideoutSystem : public System, public IMsgReceiver
{
    public:
        HideoutSystem() = default;
        virtual ~HideoutSystem() = default;

        virtual bool assertEntity(Entity* entity);
        virtual void receiveMessage(Message msg);
        virtual void fixedUpdate();
        virtual void start();
    protected:
    private:
        Hideout* hideoutPtr;
        Player* playerPtr;
        std::vector<Message> messages;
};

#endif /* !HIDEOUTSYSTEM_HPP_ */
