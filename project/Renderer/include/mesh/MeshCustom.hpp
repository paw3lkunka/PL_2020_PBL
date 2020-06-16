#ifndef _MESHCUSTOM_HPP
#define _MESHCUSTOM_HPP

#include "mesh/Mesh.hpp"
#include "mesh/MeshDataStructures.inl"

#include <vector>

/**
 * @brief Class encapsulating vertex and index data into a single mesh to draw.
 */
class MeshCustom : public Mesh
{
public:
    //TODO Documentation
    MeshCustom(std::vector<Vertex> vertices, std::vector<unsigned int> indices, Bounds bounds, std::string meshFilePath, std::string meshPath, bool customName);
    /**
     * @brief Construct a new Mesh object by default (we need this for MessageBus for some reason)
     */
    MeshCustom() = default;
    /**
     * @brief Destroy the Mesh object
     */
    virtual ~MeshCustom();

    /**
     * @brief Create buffers and fill them with mesh data
     */
    void setup();
    /**
     * @brief Bind buffers and render normally
     */
    void render();
    /**
     * @brief Render all the mesh instances
     */
    void renderInstanced(int count, glm::mat4* instanceTransforms);

    /**
     * @brief Get the Vertices vector
     * 
     * @return std::vector<Vertex> 
     */
    virtual std::vector<Vertex>* getVertices();

    /**
     * @brief Get the Indices vector
     * 
     * @return std::vector<unsigned int> 
     */
    virtual std::vector<unsigned int>* getIndices();

private:
    /**
     * @brief Clean created buffers
     */
    void clean();

    GLuint vao, vbo, ebo;
    GLuint instanceVbo;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};

#endif // _MESHCUSTOM_HPP
