#ifndef _MESHRENDERERSYSTEM_HPP
#define _MESHRENDERERSYSTEM_HPP

#include "System.hpp"

struct MeshRenderer;
struct Transform;
class Message;


    //TODO documentation
class MeshRendererSystem : public System
{
public:
    MeshRendererSystem() = default;
    virtual ~MeshRendererSystem() = default;

    //TODO documentation
    virtual bool assertEntity(Entity* entity);
    //TODO documentation
    virtual void frameUpdate();

private:
    Transform* transform;
    MeshRenderer* meshRenderer;
};

#endif // _MESHRENDERERSYSTEM_HPP
