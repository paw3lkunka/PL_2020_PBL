#ifndef _TERRAINRENDERER_INL
#define _TERRAINRENDERER_INL

#include "Component.hpp"
#include "Material.hpp"
#include "Mesh.hpp"

/**
 * @brief Component for handling advanced terrain chunk rendering
 */
struct TerrainRenderer : public Component
{
    TerrainRenderer() = default;
    virtual ~TerrainRenderer() = default;

    /// @brief Terrain's material
    Material* material;

    /// @brief Pointers to all terrain meshes
    std::vector<Mesh*> terrainMeshes;
    /// @brief Terrain transformations
    std::vector<glm::mat4> modelMatrices;
};

#endif // _TERRAINRENDERER_INL
