#ifndef _TERRAINPACKET_HPP
#define _TERRAINPACKET_HPP

#include "RenderPacket.inl"
#include "Texture.hpp"

/**
 * @brief Terrain rendering packet abstraction class
 */
struct TerrainPacket : public RenderPacket
{
    TerrainPacket(Mesh* mesh, Material* material, Texture* splatmap, glm::mat4 model) 
        : RenderPacket(mesh, material), splatmap(splatmap), modelMatrix(model) {}
    TerrainPacket() = default;
    virtual ~TerrainPacket() = default;

    void render(glm::mat4& VP);
    void renderWithShader(Shader* shader, glm::mat4& VP);

    Texture* splatmap;
    glm::mat4 modelMatrix;
};

#endif // _TERRAINPACKET_HPP
