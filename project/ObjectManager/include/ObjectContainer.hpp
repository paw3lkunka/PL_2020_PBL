#ifndef OBJECTCONTAINER_HPP_
#define OBJECTCONTAINER_HPP_

#include <vector>

class Entity;
class Component;
class Shader;
class Mesh;
class Texture;
class Cubemap;
class Material;

class ObjectMaker;
class ObjectModule;
class SceneWriter;

class ObjectContainer
{
    friend class ObjectMaker;
    friend class ObjectModule;
    friend class SceneWriter;
public:
    ObjectContainer(ObjectModule* objModule);
    ObjectContainer();
    ~ObjectContainer();

protected:
private:
    ObjectModule* objModule;

    /// @brief entities container.
    std::vector<Entity> entities;
    /// @brief components container.
    std::vector<Component*> components;
    /// @brief shaders container.
    std::vector<Shader> shaders;
    /// @brief mesh container. 
    std::vector<Mesh*> meshes;
    /// @brief texture container. 
    std::vector<Texture> textures;
    ///@brief cubemaps constainer.
    std::vector<Cubemap> cubemaps;
    
    std::vector<Material> materials;
};

#endif /* !OBJECTCONTAINER_HPP_ */
