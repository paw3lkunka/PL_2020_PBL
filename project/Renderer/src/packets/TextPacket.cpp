#include "TextPacket.hpp"

#include "RendererModule.hpp"
#include "Material.hpp"

void TextPacket::render(glm::mat4& projection)
{
    glm::mat4 model = glm::mat4(1.0f);
    model[3][0] = modelMatrix[2][0];
    model[3][1] = modelMatrix[2][1];
    model[0][0] = modelMatrix[0][0];
    model[0][1] = modelMatrix[0][1];
    model[1][0] = modelMatrix[1][0];
    model[1][1] = modelMatrix[1][1];
    material->use();
    material->getShaderPtr()->setMat4("model", model);
    material->getShaderPtr()->setInt("sprite", 0);
    mesh->render();
}