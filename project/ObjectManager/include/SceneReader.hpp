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

    // * read components by type
    void readTransform(std::string name);
    void readAudioSource(std::string name);
    void readAudioListener(std::string name);
    void readCamera(std::string name);
    void readMeshRenderer(std::string name);
    void readSphereCollider(std::string name);
    void readBoxCollider(std::string name);
    void readRigidbody(std::string name);
};

#endif /* !SCENEREADER_HPP_ */
