#include "SkinnedMesh.hpp"
#include "MeshDataStructures.inl"

SkinnedMesh::SkinnedMesh(std::vector<SkinnedVertex> vertices, std::vector<unsigned int> indices)
{
    this->vertices = vertices;
    this->indices = indices;
}