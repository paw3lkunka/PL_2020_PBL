#ifndef _BILLBOARDRENDERERSYSTEM_HPP
#define _BILLBOARDRENDERERSYSTEM_HPP

#include "System.hpp"
#include "Camera.inl"
#include "BillboardRenderer.inl"
#include "Transform.inl"
#include "Message.inl"

class BillboardRendererSystem : public System
{
public:
    BillboardRendererSystem() = default;
    virtual ~BillboardRendererSystem() = default;

    virtual bool assertEntity(Entity* entity);
    virtual void frameUpdate();

private:
    glm::quat* cameraRotation;

    Transform* transform;
    BillboardRenderer* billboardRenderer;
};

#endif // _BILLBOARDRENDERERSYSTEM_HPP
