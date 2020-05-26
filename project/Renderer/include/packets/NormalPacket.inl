#ifndef _NORMALPACKET_INL
#define _NORMALPACKET_INL

#include "RenderPacket.inl"

// TODO Documentation
struct NormalPacket : RenderPacket
{
    NormalPacket(Mesh* mesh, Material* material, glm::mat4 model) : RenderPacket(mesh, material), modelMatrix(model) {}
    NormalPacket() = default;
    virtual ~NormalPacket() = default;

    void render(glm::mat4& VP)
    {
        material->use();
        // TODO: switching between diferrent matrix usage
        material->setModel(modelMatrix);
        material->setMVP(modelMatrix, VP);
        mesh->render();
    }

    void renderWithShader(Shader* shader, glm::mat4& VP)
    {
        shader->use();
        glm::mat4 MVP = VP * modelMatrix;
        shader->setMat4("model", modelMatrix);
        shader->setMat4("MVP", MVP);
        mesh->render();
    }

    glm::mat4 modelMatrix;
};

#endif // _NORMALPACKET_INL
