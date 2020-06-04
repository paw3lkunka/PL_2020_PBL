#ifndef AUDIOSOURCESYSTEM_HPP_
#define AUDIOSOURCESYSTEM_HPP_

#include <glm/gtx/quaternion.hpp>

#include "System.hpp"

class Entity;

struct AudioSource;
struct Transform;
struct Rigidbody;

//TODO documentation
class AudioSourceSystem : public System
{
public:
    AudioSourceSystem() = default;
    virtual ~AudioSourceSystem() = default;

    virtual bool assertEntity(Entity* entity);
    virtual void start();
    virtual void frameUpdate();

protected:
private:
    AudioSource* audioSource;
    Transform* transform;
    Rigidbody* rigidbody;
};

#endif /* !AUDIOSOURCESYSTEM_HPP_ */
