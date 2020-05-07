#ifndef _MESH_HPP
#define _MESH_HPP

#include "MeshDataStructures.inl"
#include "ISerializable.inl"
#include "IFileSystem.inl"

class Mesh : public IFileSystem, public ISerializable
{
public:

    //TODO Documentation
    Mesh(std::string meshFilePath, std::string meshPath) : IFileSystem(meshFilePath), meshPath(meshPath) { }

    /**
     * @brief Pure virtual render function for use with various render calls 
     */
    virtual void render() = 0;
    /**
     * @brief Pure virtual render function for use with instanced rendering
     */
    virtual void renderInstanced(int count) {}

    //TODO Documentation
    std::string getMeshPath() { return meshPath; }
private:
    /**
     * @brief path of read mesh
     */
    std::string meshPath;
};

#endif // _MESH_HPP
