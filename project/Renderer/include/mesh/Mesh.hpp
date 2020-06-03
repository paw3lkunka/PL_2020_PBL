#ifndef _MESH_HPP
#define _MESH_HPP

#include "mesh/MeshDataStructures.inl"
#include "ISerializable.hpp"
#include "IFileSystem.inl"

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <vector>

class Mesh : public IFileSystem, public ISerializable
{
public:

    /**
     * @brief Construct a new Mesh object
     * 
     * @param meshFilePath 
     * @param meshPath 
     */
    Mesh(std::string meshFilePath, std::string meshPath, bool serialize = true);

    /**
     * @brief Pure virtual render function for use with various render calls 
     */
    virtual void render() = 0;

    /**
     * @brief Virtual render function for use with instanced rendering
     */
    virtual void renderInstanced(int count, glm::mat4* instanceTransforms) {}
    
    /**
     * @brief Get the Mesh path
     * 
     * @return std::string The path of the referenced mesh
     */
    std::string getMeshPath() { return meshPath; }

    /**
     * @brief Get the id of the mesh
     * 
     * @return unsigned int Mesh ID
     */
    unsigned int getID() { return ID; }

    /**
     * @brief Get the Vertices vector
     * 
     * @return std::vector<Vertex> 
     */
    virtual std::vector<Vertex>* getVertices() { return nullptr; }

    /**
     * @brief Get the Indices vector
     * 
     * @return std::vector<unsigned int> 
     */
    virtual std::vector<unsigned int>* getIndices() { return nullptr; }

    /**
     * @brief Bounds of the mesh
     */
    Bounds bounds;

private:
    static unsigned int idcount;
    unsigned int ID;
    /**
     * @brief path of read mesh
     */
    std::string meshPath;
};

#endif // _MESH_HPP
