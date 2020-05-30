#include "InstancedPacket.hpp"

#include "RendererModule.hpp"

InstancedPacket::InstancedPacket(Mesh* mesh, Material* material) : RenderPacket(mesh, material) 
{
    instanceMatrices.reserve(INSTANCE_ALLOCATION);
}

void InstancedPacket::render(glm::mat4& VP)
{
    if (material->getID() != RendererModule::lastMatID)
    {
        material->use();
        RendererModule::lastMatID = material->getID();
    }
    // TODO: sending MVP instead of model matrices
    mesh->renderInstanced(instanceMatrices.size(), instanceMatrices.data());
}

void InstancedPacket::renderWithShader(Shader* shader, glm::mat4& VP)
{
    shader->use();
    mesh->renderInstanced(instanceMatrices.size(), instanceMatrices.data());
}