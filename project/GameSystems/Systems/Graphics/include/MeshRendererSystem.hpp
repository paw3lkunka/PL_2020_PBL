#ifndef _MESHRENDERERSYSTEM_HPP
#define _MESHRENDERERSYSTEM_HPP

#include "System.hpp"
#include "mesh/Mesh.hpp"
#include "Material.hpp"

struct MeshRenderer;
struct Transform;
class Message;


    //TODO documentation
class MeshRendererSystem : public System
{
public:
    MeshRendererSystem() : System(false) {}
    virtual ~MeshRendererSystem() = default;

protected:
    virtual bool assertEntity(Entity* entity);
    virtual void frameUpdate();

private:
    Transform* transform;
    MeshRenderer* meshRenderer;
};

#endif // _MESHRENDERERSYSTEM_HPP
