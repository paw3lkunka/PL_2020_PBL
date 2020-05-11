#ifndef _MESH_HPP
#define _MESH_HPP

#include "mesh/MeshDataStructures.inl"
#include "ISerializable.inl"
#include "IFileSystem.inl"

class Mesh : public IFileSystem, public ISerializable
{
public:

    /**
     * @brief Construct a new Mesh object
     * 
     * @param meshFilePath 
     * @param meshPath 
     */
    Mesh(std::string meshFilePath, std::string meshPath);

    /**
     * @brief Pure virtual render function for use with various render calls 
     */
    virtual void render() = 0;
    
    /**
     * @brief Virtual render function for use with instanced rendering
     */
    virtual void renderInstanced(int count) {}
    
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

private:
    static unsigned int idcount;
    unsigned int ID;
    /**
     * @brief path of read mesh
     */
    std::string meshPath;
};

#endif // _MESH_HPP
