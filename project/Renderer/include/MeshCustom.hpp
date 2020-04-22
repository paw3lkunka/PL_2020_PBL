#ifndef _MESHCUSTOM_HPP
#define _MESHCUSTOM_HPP

#include "Mesh.inl"
#include "MeshDataStructures.inl"

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <vector>

/**
 * @brief Class encapsulating vertex and index data into a single mesh to draw.
 */
class MeshCustom : public Mesh
{
public:
    /**
     * @brief Construct a new Mesh object
     * 
     * @param vertices Vector of vertices which the mesh contains
     * @param indices Vector of indices for indexed rendering
     */
    MeshCustom(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
    /**
     * @brief Construct a new Mesh object by default (we need this for MessageBus for some reason)
     */
    MeshCustom() = default;
    /**
     * @brief Destroy the Mesh object
     */
    ~MeshCustom() = default;

    /**
     * @brief Create buffers and fill them with mesh data
     */
    void setup();
    /**
     * @brief Bind buffers and render normally
     */
    virtual void render();

    virtual void renderInstanced() {}

private:
    GLuint vao, vbo, ebo;
    Bounds bounds;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};

#endif // _MESHCUSTOM_HPP
