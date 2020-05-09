#include <glm/glm.hpp>

/**
 * @brief Structure encapsulating force it's point of application;
 * Forces are applied by kinematic system in only in one frame 
 */
struct Impulse
{
    ///@brief Force in world space.
    glm::vec3 force = {0.0f, 0.0f, 0.0f};
    ///@brief Application point of force about centre of rigidbody in world space.
    glm::vec3 point = {0.0f, 0.0f, 0.0f};
};
