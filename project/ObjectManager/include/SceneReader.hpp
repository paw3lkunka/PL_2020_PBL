#ifndef SCENEREADER_HPP_
#define SCENEREADER_HPP_

#include <json.hpp>
#include <string>

class AssetReader;
class ObjectModule;

class SceneReader
{
public:
    SceneReader(ObjectModule* objModPtr);
    ~SceneReader() = default;

    void readScene(std::string filePath);
protected:
private:
    ObjectModule* objModulePtr;

    static bool hasInstance;

    nlohmann::json j;

    void readShaders();
    void readTextures();
    void readCubemaps();
    void readMaterials();
    void readMeshes();
    void readComponents();
    void readEntities();

    std::string setName(std::string name, int index);
};

#endif /* !SCENEREADER_HPP_ */
