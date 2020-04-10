#ifndef _MESH_HPP
#define _MESH_HPP

#include "MeshDataStructures.inl"

#include <glm/glm.hpp>
#include <vector>

/**
 * @brief Class encapsulating vertex and index data into a single mesh to draw.
 * @tparam V Struct containing vertex data, to enable diferrent vertex attributes
 */
template<typename V>
class Mesh
{
public:
    std::vector<V> vertices;
    std::vector<unsigned int> indices;

    /**
     * @brief Construct a new Mesh object
     * 
     * @param vertices Vector of vertices which the mesh contains
     * @param indices Vector of indices for indexed rendering
     */
    Mesh(std::vector<V> vertices, std::vector<unsigned int> indices) : vertices(vertices), indices(indices) {}
    /**
     * @brief Destroy the Mesh object
     * 
     */
    ~Mesh();
};

#endif // _MESH_HPP
