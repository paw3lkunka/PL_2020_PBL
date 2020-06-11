#ifndef PHYSICEVENTLISTENER_HPP_
#define PHYSICEVENTLISTENER_HPP_

#include <reactphysics3d/reactphysics3d.h>

class PhysicEventListener : public rp3d::EventListener
{
    public:
        PhysicEventListener() = default;
        virtual ~PhysicEventListener() = default;

        virtual void onContact(const rp3d::CollisionCallback::CallbackData& callbackData);
        virtual void onTrigger(const rp3d::OverlapCallback::CallbackData& callbackData);
        
    protected:
    private:
};

#endif /* !PHYSICEVENTLISTENER_HPP_ */
