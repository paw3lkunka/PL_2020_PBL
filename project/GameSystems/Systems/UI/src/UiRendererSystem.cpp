#include "UiRendererSystem.hpp"

#include "Entity.hpp"
#include "Core.hpp"
#include "UiRenderer.inl"
#include "TextRenderer.inl"
#include "RectTransform.inl"
#include "Message.inl"

bool UiRendererSystem::assertEntity(Entity* entity)
{
    uiRenderer = entity->getComponentPtr<UiRenderer>();
    textRenderer = entity->getComponentPtr<TextRenderer>();
    rectTransform = entity->getComponentPtr<RectTransform>();
    return ((uiRenderer != nullptr || textRenderer != nullptr) && rectTransform != nullptr);
}

void UiRendererSystem::frameUpdate()
{
    if (uiRenderer != nullptr)
    {
        uiRenderer->modelMatrix = rectTransform->modelMatrix;
        GetCore().messageBus.sendMessage(Message(Event::RENDERER_ADD_UI_TO_QUEUE, uiRenderer));
        uiRenderer = nullptr;
    }
    if (textRenderer != nullptr)
    {
        textRenderer->modelMatrix = rectTransform->modelMatrix;
        GetCore().messageBus.sendMessage(Message(Event::RENDERER_ADD_UI_TO_QUEUE, textRenderer));
        textRenderer = nullptr;
    }
}