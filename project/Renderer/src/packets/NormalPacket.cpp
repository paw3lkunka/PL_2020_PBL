#include "NormalPacket.hpp"

#include "RendererModule.hpp"

void NormalPacket::render(glm::mat4& VP)
{
    material->use();
    // TODO: switching between diferrent matrix usage
    material->setModel(modelMatrix);
    material->setMVP(modelMatrix, VP);
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