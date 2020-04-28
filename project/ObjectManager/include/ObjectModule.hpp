#ifndef OBJECTMODULE_HPP_
#define OBJECTMODULE_HPP_

#include <vector>
#include <assimp/scene.h>

class Entity;

#include "ObjectContainer.hpp"
#include "ObjectMaker.hpp"
#include "AssetReader.hpp"
#include "SceneReader.hpp"
#include "SceneWriter.hpp"

/**
 * @brief Stores all data of Entities, Components, Shaders and materials
 * 
 */
class ObjectModule
{
    friend class ObjectMaker;
    friend int main();
public: 
    /**
     * @brief Construct a new Object Module object
     */
    ObjectModule() : objectMaker(this), objectContainer(this), assetReader(this), sceneWriter(this) {}
    virtual ~ObjectModule() = default;

    std::vector<Entity>* getEntitiesVector();
//private:    
    ObjectMaker objectMaker;
    ObjectContainer objectContainer;
    AssetReader assetReader;
    SceneReader sceneReader;
    SceneWriter sceneWriter;
};

#include "ObjectMaker.ipp"

#endif /* !OBJECTMODULE_HPP_ */
