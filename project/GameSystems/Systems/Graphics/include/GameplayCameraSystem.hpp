#ifndef _GAMEPLAYCAMERASYSTEM_HPP
#define _GAMEPLAYCAMERASYSTEM_HPP

#include <glm/glm.hpp>
#include <glm/gtx/vec_swizzle.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "System.hpp"
#include "IMsgReceiver.inl"

struct GameplayCamera;
struct Message;
struct Transform;

class GameplayCameraSystem : public System, public IMsgReceiver
{
public:
    GameplayCameraSystem() = default;
    ~GameplayCameraSystem() = default;

    virtual bool assertEntity(Entity* entity);
    virtual void receiveMessage(Message msg);
    virtual void fixedUpdate();

private:
    GameplayCamera* gameplayCamera;
    Transform* transform;

    bool usingMouse = false;
    glm::vec2 mouseInput = { 0.0f, 0.0f };
    float scrollInput = 0.0f;
};

#endif // _GAMEPLAYCAMERASYSTEM_HPP
