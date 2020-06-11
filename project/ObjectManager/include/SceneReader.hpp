#ifndef SCENEREADER_HPP_
#define SCENEREADER_HPP_

#include <json.hpp>
#include <string>

class AssetReader;
class ObjectModule;
struct Component;
struct Button;

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
    void readCubemapsHdr();
    void readMaterials();
    void readMeshes();
    void readFonts();
    void readComponents();
    void readEntities();

    std::string setName(std::string name, int index);

    // * read components by type
    void readTransform(std::string name);
    void readAudioSource(std::string name);
    void readAudioListener(std::string name);
    void readCamera(std::string name);
    void readMeshRenderer(std::string name);
    void readTerrainRenderer(std::string name);
    void readSphereCollider(std::string name);
    void readBoxCollider(std::string name);
    void readRigidbody(std::string name);
    void readLight(std::string name);
    void readPhysicalInputKeymap(std::string name);
    void readPaddle(std::string name);
    void readBone(std::string name);
    void readSkeleton(std::string name);
    void readHydroBody(std::string name);
    void readHydroSurface(std::string name);
    void readHydroAccelerator(std::string name);
    void readKayak(std::string name);
    void readHideout(std::string name);
    void readEnemy(std::string name);
    void readEnemyAnimation(std::string name);
    void readUiRenderer(std::string name);
    void readTextRenderer(std::string name);
    void readRectTransform(std::string name);
    void readButton(std::string name);
    void readUiSortingGroup(std::string name);

    void assignToEntity(std::string name, Component* component);
    void readTransformParents(std::string name);
    void readRectTransformParents(std::string name);
    void readButtonEvents(std::string name, Button* buttonPtr);
};

#endif /* !SCENEREADER_HPP_ */
