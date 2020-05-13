#ifndef SKINNEDMESH_HPP_
#define SKINNEDMESH_HPP_

#include "Mesh.inl"
#include "MeshDataStructures.inl"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>

struct VertexSkinned;

//TODO: Fill this class with things u need
class MeshSkinned : public Mesh
{
public:
    //TODO Documentation
    MeshSkinned(std::vector<VertexSkinned> vertices, std::vector<unsigned int> indices, Bounds bounds, std::string meshFilePath, std::string meshPath);
    MeshSkinned() = default;
    ~MeshSkinned() = default;

    //TODO Documentation
    void setup();
    //TODO Documentation
    void render();
    //TODO Documentation
    virtual void renderInstanced() {}

protected:
private:
    GLuint vao, vbo, ebo;
    Bounds bounds;
    std::vector<VertexSkinned> vertices;
    std::vector<unsigned int> indices;
};

#endif /* !SKINNEDMESH_HPP_ */
