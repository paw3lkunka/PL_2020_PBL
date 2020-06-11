#ifndef PHYSICEVENTLISTENER_HPP_
#define PHYSICEVENTLISTENER_HPP_

#include <reactphysics3d/reactphysics3d.h>

class PhysicEventListener : public rp3d::EventListener
{
    public:
        PhysicEventListener() = default;
        virtual ~PhysicEventListener() = default;

        virtual void onContact(const CollisionCallback::CallbackData& callbackData);
        
    protected:
    private:
};

#endif /* !PHYSICEVENTLISTENER_HPP_ */
