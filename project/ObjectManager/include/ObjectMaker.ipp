#include "ObjectMaker.hpp"
#include "Entity.hpp"
#include "MeshDataStructures.inl"
#include "Components.inc"
#include <typeinfo>
#include <string>

template<typename T>
T* ObjectMaker::newEmptyComponent()
{
    objContainer->components.push_back(new T());
    return dynamic_cast<T*>(objContainer->components[objContainer->components.size() - 1]);
}

template<typename T>
T* ObjectMaker::newEmptyComponentForLastEntity()
{
    objContainer->components.push_back(new T());
    objContainer->entities[objContainer->entities.size() - 1].addComponent(objContainer->components[objContainer->components.size() - 1]);
    return dynamic_cast<T*>(objContainer->components[objContainer->components.size() - 1]);
}

template<typename T, typename U>
T* ObjectMaker::newMesh(std::vector<U> vertices, std::vector<unsigned int> indices, Bounds bounds, std::string& filePath, std::string meshName)
{
    if(std::string(typeid(T).name()).find("Mesh") != std::string::npos)
    {
        objContainer->meshes.push_back(new T(vertices, indices, bounds, filePath, filePath + "/" + meshName));
        return dynamic_cast<T*>(objContainer->meshes[objContainer->meshes.size() - 1]);
    }
    return nullptr;
}