#include "TerrainPacket.hpp"

#include "Material.hpp"

void TerrainPacket::render(glm::mat4& VP)
{
    material->use();
    int texUnit = material->getTextureUnit("splatmap");
    splatmap->bind(texUnit);
    material->getShaderPtr()->setInt("splatmap", texUnit);
    material->setTransformMatrices(modelMatrix, VP);
    mesh->render();
}

void TerrainPacket::renderWithShader(Shader* shader, glm::mat4& VP)
{
    shader->use();
    glm::mat4 MVP = VP * modelMatrix;
    shader->setMat4("model", modelMatrix);
    shader->setMat4("MVP", MVP);
    mesh->render();
}