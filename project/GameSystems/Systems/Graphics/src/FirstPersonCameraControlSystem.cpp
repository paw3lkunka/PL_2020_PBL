#include "FirstPersonCameraControlSystem.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "Core.hpp"
#include "Entity.hpp"
#include "Message.inl"
#include "MouseDataStructures.inl"

#include "Transform.inl"
#include "Camera.inl"

bool FirstPersonCameraControlSystem::assertEntity(Entity* entity)
{
    camera = entity->getComponentPtr<Camera>();
    if(camera == nullptr)
    {
        return false;
    }

    transform = entity->getComponentPtr<Transform>();
    if(transform == nullptr)
    {
        return false;
    }

    return (camera->isMain && camera->control == CameraControl::FirstPerson);
}

void FirstPersonCameraControlSystem::receiveMessage(Message msg)
{

}

void FirstPersonCameraControlSystem::fixedUpdate()
{

}
