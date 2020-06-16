#include "mesh/Mesh.hpp"

unsigned int Mesh::idcount = 0;

Mesh::Mesh(std::string meshFilePath, std::string meshPath, bool customName, bool serialize) 
    : ISerializable(serialize), IFileSystem(meshFilePath), meshPath(meshPath), customName(customName)
{
    ID = idcount;
    idcount++;
}