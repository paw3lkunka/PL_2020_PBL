#ifndef _INSTANCEDPACKET_INL
#define _INSTANCEDPACKET_INL

#include <glad/glad.h>
#include <vector>

#include "RenderPacket.inl"

// TODO Documentation
struct InstancedPacket : RenderPacket
{
    static constexpr unsigned int INSTANCE_ALLOCATION = 32;

    InstancedPacket(Mesh* mesh, Material* material) : RenderPacket(mesh, material) 
    {
        instanceMatrices.reserve(INSTANCE_ALLOCATION);
    }
    InstancedPacket() = default;
    virtual ~InstancedPacket() = default;

    void render(glm::mat4& VP)
    {
        material->use();
        // TODO: sending MVP instead of model matrices
        mesh->renderInstanced(instanceMatrices.size(), instanceMatrices.data());
    }

    void renderWithShader(Shader* shader, glm::mat4& VP)
    {
        shader->use();
        mesh->renderInstanced(instanceMatrices.size(), instanceMatrices.data());
    }

    std::vector<glm::mat4> instanceMatrices;
};

#endif // _INSTANCEDPACKET_INL
