#include "NormalPacket.hpp"

void NormalPacket::render(glm::mat4& VP)
{
    material->use();
    material->setTransformMatrices(modelMatrix, VP);
    mesh->render();
}

void NormalPacket::renderWithShader(Shader* shader, glm::mat4& VP)
{
    shader->use();
    glm::mat4 MVP = VP * modelMatrix;
    shader->setMat4("model", modelMatrix);
    shader->setMat4("MVP", MVP);
    mesh->render();
}