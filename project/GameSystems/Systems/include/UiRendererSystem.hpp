#ifndef _UIRENDERERSYSTEM_HPP
#define _UIRENDERERSYSTEM_HPP

#include "System.hpp"
#include "mesh/UiQuad.hpp"
#include "Material.hpp"

struct UiRenderer;
struct RectTransform;
class Message;

class UiRendererSystem : public System
{
public:
    UiRendererSystem() = default;
    virtual ~UiRendererSystem() = default;

protected:
    virtual bool assertEntity(Entity* entity);
    virtual void frameUpdate();

private:
    RectTransform* rectTransform;
    UiRenderer* uiRenderer;
};

#endif // _UIRENDERERSYSTEM_HPP
