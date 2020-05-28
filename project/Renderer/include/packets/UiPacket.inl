#ifndef _UIPACKET_INL
#define _UIPACKET_INL

#include "Material.hpp"
#include "UiQuad.hpp"

struct UiPacket
{
    UiPacket(UiQuad* mesh, Material* material, glm::mat3& modelMatrix) 
        : mesh(mesh), material(material), modelMatrix(modelMatrix) {}
    UiPacket() = default;
    virtual ~UiPacket() = default;

    void render(glm::mat4 projection)
    {
        material->use();
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, character);
        //material->getShaderPtr()->setInt("text", 0);
        material->setMat4("projection", projection);
        glm::mat4 model = glm::mat4(1.0f);
        model[3][0] = modelMatrix[2][0];
        model[3][1] = modelMatrix[2][1];
        model[0][0] = modelMatrix[0][0];
        model[0][1] = modelMatrix[0][1];
        model[1][0] = modelMatrix[1][0];
        model[1][1] = modelMatrix[1][1];
        material->setModel(model);
        //material->getShaderPtr()->setMat3("model", modelMatrix);
        mesh->render();
    }

    glm::mat3 modelMatrix;

    UiQuad* mesh;
    Material* material;
};

#endif // _UIPACKET_INL
