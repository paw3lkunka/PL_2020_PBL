#include "CameraSystem.hpp"

#include <glm/gtx/string_cast.hpp>

#include "Core.hpp"
#include "Entity.hpp"

#include "Camera.inl"
#include "Transform.hh"
#include "Message.inl"
#include "GamepadDataStructures.inl"

Camera* CameraSystem::mainCamera = nullptr;

void CameraSystem::receiveMessage(Message msg)
{
    switch (msg.getEvent())
    {
        case Event::WINDOW_RESIZED:
            {
                glm::ivec2 window = msg.getValue<glm::ivec2>();
                mainCamera->projectionChanged = true;
                mainCamera->getFrustumModifiable().aspectRatio = (float)window.x / (float)window.y;
            }
            break;

        case Event::KEY_PRESSED:
            {
                switch(msg.getValue<int>())
                {
                    case GLFW_KEY_F1:
                        switchDebugCameraControl = !switchDebugCameraControl;
                        break;

                    case GLFW_KEY_C:
                        switchGameplayCameraControl = !switchGameplayCameraControl;
                        break;
                }
            }
            break;

        case Event::GAMEPAD_BUTTON_PRESSED:
            {
                GamepadButtonData data = msg.getValue<GamepadButtonData>();
                switch(data.buttonId)
                {
                    case GLFW_GAMEPAD_BUTTON_BACK:
                        switchGameplayCameraControl = !switchGameplayCameraControl;
                }
            }
            break;
    }
}

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
        mainCamera = cameraTmp;
        GetCore().messageBus.sendMessage(Message(Event::RENDERER_SET_MAIN_CAMERA, mainCamera));
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
        // * ===== Calculate projection matrix =====
        if (camera->projectionChanged)
        {
            ViewFrustum& frustum = camera->getFrustumModifiable();

            switch (camera->getProjectionMode())
            {
                case CameraProjection::Perspective:
                    camera->projectionMatrix = glm::perspective(
                                                    glm::radians(frustum.fieldOfView),
                                                    frustum.aspectRatio,
                                                    frustum.nearPlane,
                                                    frustum.farPlane
                                                    );
                    break;
                // ! ----- Warning! Orthographic projection is untested and may produce undefined behaviour ----- !
                case CameraProjection::Orthographic:
                    camera->projectionMatrix = glm::ortho(
                                                    -frustum.orthographicSize * frustum.aspectRatio,
                                                    frustum.orthographicSize * frustum.aspectRatio,
                                                    -frustum.orthographicSize,
                                                    frustum.orthographicSize,
                                                    frustum.nearPlane,
                                                    frustum.farPlane
                                                    );
                    break;
            }

            // * ===== Calculate frustum properties used for frustum culling =====
            
            float tang = glm::tan(glm::radians(frustum.fieldOfView / 2.0f));
            frustum.Hnear = 2.0f * tang * frustum.nearDist;
            frustum.Wnear = frustum.Hnear * frustum.aspectRatio;

            frustum.Hfar = 2.0f * tang * frustum.farDist;
            frustum.Wfar = frustum.Hfar * frustum.aspectRatio;
        }

        // * ===== Calculate view matrix =====
        camera->viewMatrix = transform->getToModelMatrix();
        // * ===== Update camera frustum =====
        ViewFrustum& frustum = camera->getFrustumModifiable();
        frustum.position = glm::vec3(transform->getModelMatrix()[3]);
            // std::cout << "Cam pos: " << glm::to_string(frustum.position) << '\n';
        frustum.front = -glm::normalize(glm::vec3(transform->getModelMatrix()[2]));
            // std::cout << "Cam front: " << glm::to_string(glm::vec3(transform->getModelMatrix()[2])) << '\n';
            // std::cout << "Cam front normalized: " << glm::to_string(frustum.front) << '\n';
        frustum.up = glm::normalize(glm::vec3(transform->getModelMatrix()[1]));
            // std::cout << "Cam up: " << glm::to_string(glm::vec3(transform->getModelMatrix()[1])) << '\n';
            // std::cout << "Cam up normalized: " << glm::to_string(frustum.up) << '\n';
        frustum.right = glm::normalize(glm::vec3(transform->getModelMatrix()[0]));
            // std::cout << "Cam right: " << glm::to_string(glm::vec3(transform->getModelMatrix()[0])) << '\n';
            // std::cout << "Cam right normalized: " << glm::to_string(frustum.right) << '\n';
        
        camera->projectionChanged = false;
    }

    if(switchDebugCameraControl)
    {
        CameraControl& control = camera->control;
        if(control == CameraControl::Free)
        {
            control = CameraControl::ThirdPerson;
        }
        else
        {
            control = CameraControl::Free;
        }
        

        switchDebugCameraControl = false;
    }

    if(switchGameplayCameraControl)
    {
        CameraControl& control = camera->control;
        switch(control)
        {
            case CameraControl::ThirdPerson:
                control = CameraControl::FirstPerson;
                break;

            case CameraControl::FirstPerson:
                control = control = CameraControl::ThirdPerson;
                break;
        }

        switchGameplayCameraControl = false;
    }
}