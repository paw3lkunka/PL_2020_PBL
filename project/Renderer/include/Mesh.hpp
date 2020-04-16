#ifndef _MESH_HPP
#define _MESH_HPP

#include "MeshDataStructures.inl"

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <vector>

/**
 * @brief Class encapsulating vertex and index data into a single mesh to draw.
 */
class Mesh
{
public:
    /**
     * @brief Construct a new Mesh object
     * 
     * @param vertices Vector of vertices which the mesh contains
     * @param indices Vector of indices for indexed rendering
     */
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices) : vertices(vertices), indices(indices) {}
    /**
     * @brief Destroy the Mesh object
     * 
     */
    ~Mesh();

    void Setup();

private:
    GLuint vao, vbo, ebo;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};

#endif // _MESH_HPP
