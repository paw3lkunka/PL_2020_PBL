#ifndef _GAMEPLAYCAMERA_INL
#define _GAMEPLAYCAMERA_INL

#include <glm/glm.hpp>

#include "Component.hpp"
#include "Transform.inl"

struct GameplayCamera : public Component
{
    /// @brief Actual camera transform
    Transform* cameraTransform;

    /// @brief Player object camera shall follow
    Transform* playerTransform;
    
    /// @brief How fast the camera moves
    float movementSpeed = 120.0f;

    /// @brief 
    float lowerClampAngle = -20.0f;
    
    /// @brief 
    float upperClampAngle = 80.0f;

    /// @brief 
    float distanceToPlayer = 100.0f;

    /// @brief 
    float lowerClampDistance = 10.0f;

    /// @brief 
    float upperClampDistance = 200.0f;

    /// @brief 
    glm::vec3 targetOffset = { 0.0f, 0.0f, 0.0f };

    /// @brief 
    float inputSensitivity = 150.0f;

    /// @brief
    float lerpParameter = 0.9f;
};

#endif // _GAMEPLAYCAMERA_INL
