#include "mesh/Mesh.hpp"

unsigned int Mesh::idcount = 0;

Mesh::Mesh(std::string meshFilePath, std::string meshPath) : IFileSystem(meshFilePath), meshPath(meshPath)
{
    ID = idcount;
    idcount++;
}