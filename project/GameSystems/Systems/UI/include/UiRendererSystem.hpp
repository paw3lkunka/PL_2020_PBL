#ifndef _UIRENDERERSYSTEM_HPP
#define _UIRENDERERSYSTEM_HPP

#include "System.hpp"
#include "UiQuad.hpp"
#include "Material.hpp"

struct UiRenderer;
struct TextRenderer;
struct RectTransform;
class Message;

//TODO documentation
class UiRendererSystem : public System
{
public:
    UiRendererSystem() : System(false) {}
    virtual ~UiRendererSystem() = default;

protected:
    virtual bool assertEntity(Entity* entity);
    virtual void frameUpdate();

private:
    RectTransform* rectTransform;
    UiRenderer* uiRenderer;
    TextRenderer* textRenderer;
};

#endif // _UIRENDERERSYSTEM_HPP
