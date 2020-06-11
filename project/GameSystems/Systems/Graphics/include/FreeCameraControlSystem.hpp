#ifndef _FREECAMERACONTROLSYSTEM_HPP
#define _FREECAMERACONTROLSYSTEM_HPP

#include "System.hpp"
#include "IMsgReceiver.inl"

#include <glm/glm.hpp>

struct Transform;
struct Camera;


/**
 * @brief Camera control system responsible for simple input interpretation (fly around)
 * 
 */
class FreeCameraControlSystem : public System, public IMsgReceiver
{
public:
    FreeCameraControlSystem() = default;
    virtual ~FreeCameraControlSystem() = default;

    /**
     * @brief Searches for component with camera and transform
     * 
     * @param entity entity provided by system
     * @return true if entity has transform, camera and camera is main
     * @return false if above condition fails
     */
    virtual bool assertEntity(Entity* entity);
    /**
     * @brief Handles input module messages for input handling
     * 
     * @param msg Message provided by message bus
     */
    virtual void receiveMessage(Message msg);
    /**
     * @brief Function handling camera movement
     * 
     */
    virtual void fixedUpdate();

    /**
     * @brief Controlled camera speed
     */
    float speed = 1.0f;
    /**
     * @brief Mouse look sensitivity
     */
    float sensitivity = 0.1f;
    
private:
    glm::vec3 movementVector = glm::vec3(0.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    float yaw = 0.0f;
    float pitch = 0.0f;

    bool usingMouse = false;

    Transform* transform;
    Camera* camera;
};

#endif // _FREECAMERACONTROLSYSTEM_HPP
