#include "InstancedPacket.hpp"

#include "RendererModule.hpp"

InstancedPacket::InstancedPacket(Mesh* mesh, Material* material) : RenderPacket(mesh, material) 
{
    instanceMatrices.reserve(INSTANCE_ALLOCATION);
    instanceMatricesUnculled.reserve(INSTANCE_ALLOCATION);
    instanceOccluded.reserve(INSTANCE_ALLOCATION);
}

void InstancedPacket::render(glm::mat4& VP)
{
    material->use();

    // TODO: sending MVP instead of model matrices

    for (size_t i = 0; i < instanceMatrices.size(); i++)
    {
        if (!instanceOccluded[i])
        {
            instanceMatricesUnculled.push_back(*(instanceMatrices[i]));
        }
    }

    mesh->renderInstanced(instanceMatricesUnculled.size(), instanceMatricesUnculled.data());
    
    instanceMatricesUnculled.clear();
}

void InstancedPacket::renderWithShader(Shader* shader, glm::mat4& VP)
{
    shader->use();
    for (size_t i = 0; i < instanceMatrices.size(); i++)
    {
        if (!instanceOccluded[i])
        {
            instanceMatricesUnculled.push_back(*(instanceMatrices[i]));
        }
    }

    mesh->renderInstanced(instanceMatrices.size(), instanceMatricesUnculled.data());
    
    instanceMatricesUnculled.clear();
}