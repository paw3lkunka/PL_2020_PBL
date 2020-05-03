#ifndef SKINNEDMESH_HPP_
#define SKINNEDMESH_HPP_

#include "Mesh.inl"

#include <vector>

struct VertexSkinned;

//TODO: Fill this class with things u need
class MeshSkinned : public Mesh
{
public:
    MeshSkinned(std::vector<VertexSkinned> vertices, std::vector<unsigned int> indices, Bounds bounds);
    MeshSkinned() = default;
    ~MeshSkinned() = default;

    void setup();
    void render();
    virtual void renderInstanced() {}

protected:
private:
    GLuint vao, vbo, ebo;
    Bounds bounds;
    std::vector<VertexSkinned> vertices;
    std::vector<unsigned int> indices;
};

#endif /* !SKINNEDMESH_HPP_ */
