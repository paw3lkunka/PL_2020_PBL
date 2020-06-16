#include "NormalPacket.hpp"
#include "Material.hpp"

void NormalPacket::render(glm::mat4& VP)
{
    material->use();
    material->setTransformMatrices(modelMatrix, VP);
    mesh->render();
}

void NormalPacket::renderWithShader(Shader* shader, glm::mat4& VP)
{
    shader->use();
    // HACK: Setting diffuse is hard coded for ssao rendering
    shader->setInt("diffuse", 0);
    auto tex = material->getTexturePtr("diffuse");
    if (tex != nullptr)
    {
        tex->bind(0);
    }
    glm::mat4 MVP = VP * modelMatrix;
    shader->setMat4("model", modelMatrix);
    shader->setMat4("MVP", MVP);
    mesh->render();
}