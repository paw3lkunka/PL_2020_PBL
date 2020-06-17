#ifndef _TERRAINRENDERERSYSTEM_HPP
#define _TERRAINRENDERERSYSTEM_HPP

#include "System.hpp"

struct TerrainRenderer;
struct Transform;
class Message;

/**
 * @brief System for handling terrain rendering in renderer module
 */
class TerrainRendererSystem : public System
{
public:
    TerrainRendererSystem() : System(false) {}
    virtual ~TerrainRendererSystem() = default;

protected:
    virtual bool assertEntity(Entity* entity);
    virtual void frameUpdate();

private:
    Transform* transform;
    TerrainRenderer* terrainRenderer;
};

#endif // _TERRAINRENDERERSYSTEM_HPP
