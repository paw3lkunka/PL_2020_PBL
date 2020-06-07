#ifndef AUDIOLISTENERSYSTEM_HPP_
#define AUDIOLISTENERSYSTEM_HPP_

#include <glm/gtx/quaternion.hpp>

#include "System.hpp"

struct Entity;

struct AudioListener;
struct Transform;
struct Rigidbody;

//TODO documentation
class AudioListenerSystem : public System
{
    public:
        AudioListenerSystem() = default;
        virtual ~AudioListenerSystem() = default;
        virtual bool assertEntity(Entity* entity);
        
        virtual void start();
        virtual void frameUpdate();

    protected:
    private:
        AudioListener* audioListener;
        Transform* transform;
        Rigidbody* rigidbody;
};

#endif /* !AUDIOLISTENERSYSTEM_HPP_ */