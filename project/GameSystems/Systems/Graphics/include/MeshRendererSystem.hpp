#ifndef _MESHRENDERERSYSTEM_HPP
#define _MESHRENDERERSYSTEM_HPP

#include "System.hpp"
#include "Mesh.hpp"
#include "Material.hpp"

struct MeshRenderer;
struct Transform;
class Message;

/**
 * @brief System for standard mesh sending info to renderer module
 */
class MeshRendererSystem : public System
{
public:
    MeshRendererSystem() = default;
    virtual ~MeshRendererSystem() = default;

protected:
    virtual bool assertEntity(Entity* entity);
    virtual void frameUpdate();

private:
    Transform* transform;
    MeshRenderer* meshRenderer;
};

#endif // _MESHRENDERERSYSTEM_HPP
