#ifndef _MESHRENDERERSYSTEM_HPP
#define _MESHRENDERERSYSTEM_HPP

#include <unordered_map>

#include "System.hpp"
#include "Mesh.hpp"
#include "Material.hpp"

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
    // Used for instanced rendering
    // TODO: Using pointers as keys may not be the smartest idea, update to mesh and material ids 
    std::unordered_map<std::pair<Mesh*, Material*>, std::vector<glm::mat4>> meshInstances;

    Transform* transform;
    MeshRenderer* meshRenderer;
};

#endif // _MESHRENDERERSYSTEM_HPP
