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
    /// @brief Current terrain's splatmap image
    Texture* splatmap;

    /// @brief Pointer terrain's meshe
    Mesh* terrainMesh;
    /// @brief Terrain transform
    glm::mat4 modelMatrix;
};

#endif // _TERRAINRENDERER_INL
