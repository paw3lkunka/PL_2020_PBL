#include "BillboardRendererSystem.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Entity.hpp"
#include "Core.hpp"
#include "Systems.inc"

bool BillboardRendererSystem::assertEntity(Entity* entity)
{
    transform = entity->getComponentPtr<Transform>();
    billboardRenderer = entity->getComponentPtr<BillboardRenderer>();
    return (transform != nullptr && billboardRenderer != nullptr);
}

void BillboardRendererSystem::frameUpdate()
{
    // * ===== Calculates model matrix and sends it to renderer module =====
    glm::mat4 model(1);
    model = glm::translate(model, transform->getLocalPosition());
    model = model * glm::toMat4(CameraSystem::mainCameraTransform->getLocalRotation());
    model = glm::scale(model, transform->getLocalScale());

    billboardRenderer->modelMatrix = model;

    GetCore().messageBus.sendMessage(Message(Event::RENDERER_ADD_BILLBOARD_TO_QUEUE, billboardRenderer));
}