#ifndef PADDLECONTROLSYSTEM_HPP_
#define PADDLECONTROLSYSTEM_HPP_

#include "System.hpp"
#include "IMsgReceiver.inl"

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/gtx/quaternion.hpp>

struct Transform;
struct Paddle;
class Message;

class PaddleControlSystem : public System, public IMsgReceiver
{
public:
    PaddleControlSystem() = default;
    virtual ~PaddleControlSystem() = default;

    virtual void receiveMessage(Message msg);

protected:
        /**
         * @brief Checks, if given Entity contain all required components
         * implementation should also save pointers to components in class variables 
         * @param entity pointer to asserted entity
         * @returns if entity has all required components
         */
        virtual bool assertEntity(Entity* entity);

        /**
         * @brief Initializes processed entities, runs before first update
         */
        virtual void start();

        /**
         * @brief Contain logic of the system called in fixed time steps
         * called in Process() only when AssertEntity() returned true
         * should use class variables to access components
         */
        virtual void fixedUpdate() {}

        /**
         * @brief Contain logic of the system runned once per frame
         * called in Process() only when AssertEntity() returned true
         * should use class variables to access components
         */
        virtual void frameUpdate();
private:

    glm::vec3 startPosition;
    glm::quat startRotation;
    glm::vec3 lastFramePosition;

    glm::vec2 inputRaw = {0.0f, 0.0f};
    glm::vec2 interpolationTarget = {0.0f, 0.0f};

    bool keyboardInput = true;

    Transform* transformPtr;
    Paddle* paddlePtr;
};

#endif /* !PADDLECONTROLSYSTEM_HPP_ */
