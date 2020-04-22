#include "CameraSystem.hpp"

#include "Core.hpp"
#include "Entity.hpp"

#include "Camera.inl"
#include "Transform.inl"

Transform* CameraSystem::mainCameraTransform = nullptr;
Camera* CameraSystem::mainCamera = nullptr;

bool CameraSystem::assertEntity(Entity* entity)
{
    transform = entity->getComponentPtr<Transform>();
    camera = entity->getComponentPtr<Camera>();
    return (transform != nullptr && camera != nullptr);
}

bool CameraSystem::setAsMain(Entity* entity)
{
    Transform* transformTmp = entity->getComponentPtr<Transform>();
    Camera* cameraTmp = entity->getComponentPtr<Camera>();
    if (transformTmp != nullptr && cameraTmp != nullptr)
    {
        if (mainCamera != nullptr)
        {
            mainCamera->isMain = false;
        }
        cameraTmp->isMain = true;
        mainCameraTransform = transformTmp;
        mainCamera = cameraTmp;
        return true;
    }
    else
    {
        return false;
    }
}

void CameraSystem::frameUpdate()
{
    if (camera->isMain)
    {
        // TODO: Check for projection change and set dirty flag accordingly
        // * ===== Calculate and send projection matrix to renderer =====
        if (camera->projectionChanged)
        {
            float aspect = (float)Core::INIT_WINDOW_WIDTH / Core::INIT_WINDOW_HEIGHT;
            switch (camera->projectionMode)
            {
                case CameraProjection::Perspective:
                    camera->projectionMatrix = glm::perspective(
                                                    glm::radians(camera->fieldOfView),
                                                    aspect,
                                                    camera->nearPlane,
                                                    camera->farPlane
                                                    );
                    break;
                case CameraProjection::Orthographic:
                    camera->projectionMatrix = glm::ortho(
                                                    -camera->othographicSize * aspect,
                                                    camera->othographicSize * aspect,
                                                    -camera->othographicSize,
                                                    camera->othographicSize,
                                                    camera->nearPlane,
                                                    camera->farPlane
                                                    );
                    break;
            }            
            GetCore().messageBus.sendMessage(Message(Event::RENDERER_SET_PROJECTION_MATRIX, &camera->projectionMatrix));
            camera->projectionChanged = false;
        }

        // * ===== Calculate and send view matrix to renderer =====
        camera->viewMatrix = glm::inverse(transform->localToWorldMatrix);
        GetCore().messageBus.sendMessage(Message(Event::RENDERER_SET_VIEW_MATRIX, &camera->viewMatrix));
    }
}