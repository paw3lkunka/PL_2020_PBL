#ifndef KINEMATICSYSTEM_HPP_
#define KINEMATICSYSTEM_HPP_

#include "System.hpp"

class Transform;
class rigidbody

/**
 * @brief Calculate speed and position of rigidbody depending on forces, and current speed
 * 
 */
class KinematicSystem : System
{
    public:

        KinematicSystem() = default;
        virtual ~KinematicSystem() = default;

    protected:
        virtual bool assertEntity(Entity* entity);
        virtual void fixedUpdate();

    private:
        Transform* transformPtr;
        Rigidbody* rBodyPtr;

};

#endif /* !KINEMATICSYSTEM_HPP_ */
