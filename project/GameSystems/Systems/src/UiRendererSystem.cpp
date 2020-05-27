#include "UiRendererSystem.hpp"

#include "Entity.hpp"
#include "Core.hpp"
#include "UiRenderer.inl"
#include "RectTransform.inl"
#include "Message.inl"

bool UiRendererSystem::assertEntity(Entity* entity)
{
    uiRenderer = entity->getComponentPtr<UiRenderer>();
    rectTransform = entity->getComponentPtr<Transform>();
    return (uiRenderer != nullptr && rectTransform != nullptr);
}

void UiRendererSystem::frameUpdate()
{
    uiRenderer->modelMatrix = rectTransform->modelMatrix;
    GetCore().messageBus.sendMessage(Message(Event::RENDERER_ADD_UI_TO_QUEUE, uiRenderer));
}