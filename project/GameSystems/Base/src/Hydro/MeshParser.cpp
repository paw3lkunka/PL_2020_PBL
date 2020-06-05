#include "Hydro/MeshParser.hpp"

#include "Hydro/Data/HydroTriangle.hpp"
#include "Mesh.hpp"

std::vector<HydroTriangle> HydroMeshParser::parse(Mesh mesh, glm::mat4 modelMatrix)
{
    std::vector<HydroTriangle> triangles;

    auto& indices = mesh->getIndices();
    auto& vertices = mesh->getVertices();

    verticesHeights.clear();
    for(auto it = vertices.begin(); it != vertices.end(); it++)
    {
        glm::vec3 vertexWorldPosition = vertexToModel(*it.position, modelMatrix);
        // TODO: proper heights calculations
        verticesHeights.push_back(vertexWorldPosition.y);
    }

    for(auto it = indices.begin(); it != indices.end(); it += 3)
    {
        glm::vec3 A = vertexToModel(vertices[*it].position, modelMatrix);
        glm::vec3 B = vertexToModel(vertices[*(it + 1)].position, modelMatrix);
        glm::vec3 C = vertexToModel(vertices[*(it + 2)].posiiton, modelMatrix);

        triangles
            .push_back
            (
                HydroTriangle
                (
                    A, B, C, 
                    verticesHeights[*it], 
                    verticesHeights[*(it + 1)], 
                    verticesHeights[*(it + 2)]
                )
            );
    }

    return triangles;
}

glm::vec3 HydroMeshParser::vertexToModel(glm::vec3 vertex, glm::mat4 modelMatrix)
{
    return static_cast<glm::vec3>( modelMatrix * glm::vec4(vertex, 1.0f) ) 
}
