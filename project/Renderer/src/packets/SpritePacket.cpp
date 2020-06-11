#include "SpritePacket.hpp"

#include "RendererModule.hpp"

void SpritePacket::render(glm::mat4& projection)
{
    material->setMat4("projection", projection);
    material->use();
    glm::mat4 model = glm::mat4(1.0f);
    model[3][0] = modelMatrix[2][0];
    model[3][1] = modelMatrix[2][1];
    model[0][0] = modelMatrix[0][0];
    model[0][1] = modelMatrix[0][1];
    model[1][0] = modelMatrix[1][0];
    model[1][1] = modelMatrix[1][1];
    material->setModel(model);
    mesh->render();
}