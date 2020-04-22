#ifndef SKINNEDMESH_HPP_
#define SKINNEDMESH_HPP_
#include <vector>

struct SkinnedVertex;

//TODO: Fill this class with things u need
class SkinnedMesh
{
public:
    SkinnedMesh(std::vector<SkinnedVertex> vertices, std::vector<unsigned int> indices);
    ~SkinnedMesh() = default;

protected:
private:
    std::vector<SkinnedVertex> vertices;
    std::vector<unsigned int> indices;
};

#endif /* !SKINNEDMESH_HPP_ */
