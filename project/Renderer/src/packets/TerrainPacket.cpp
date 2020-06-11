#include "TerrainPacket.hpp"

void TerrainPacket::render(glm::mat4& VP)
{
    material->setTransformMatrices(modelMatrix, VP);
    int texUnit = material->getTextureUnit("splatmap");
    material->use();
    splatmap->bind(texUnit);
    material->getShaderPtr()->setInt("splatmap", texUnit);
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