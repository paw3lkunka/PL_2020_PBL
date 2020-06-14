#ifndef _RESOURCEMODULE_HPP
#define _RESOURCEMODULE_HPP

#include "IModule.inl"
#include "AudioFile.hpp"
#include "AssetStructers.inl"
#include "mesh/MeshSkinned.hpp"
#include "mesh/MeshCustom.hpp"
#include "Animation.hpp"
#include "Bone.inl"

#include <unordered_map>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <vector>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class ObjectModule;
class ObjectMaker;
class SceneModule;
class Bone;
class SceneWriter;
struct Transform;

void displayNodeHierarchy(aiNode* node, int depth = 0);
void displayGlmVec3(glm::vec3 vec);
void displayGlmMat4Decomposed(glm::mat4 mat);
void displayAssimpMat4(aiMatrix4x4 mat);
void displayGlmMat4(glm::mat4 mat);

/**
 * @brief Resource Module class for reading and storage assets data
 */
class AssetReader
{
    friend class ObjectMaker;
    friend class ObjectModule;
public:
    //TODO DOCUMENTATION
    AssetReader(ObjectModule* objModule);
    ~AssetReader() = default;

private:
    ObjectModule* objectModulePtr;

    /**
     * @brief assimp importer reference;
     */
    Assimp::Importer importer;

    static bool hasInstance;

    static unsigned int bonesAmount;

    std::unordered_map<std::string, AudioFile> audioClips;
    std::unordered_map<std::string, TextureData> textures;
    std::unordered_map<std::string, TextureHdrData> texturesHdr;
    std::unordered_map<std::string, std::string> shaders;

    // TODO: I dont have the patience to do it another way
    glm::mat4 globalInverseTransform;
    bool makeEntities = true;

    //load files to storages methods
    bool loadAudioClip(std::string path);
    bool loadTexture(std::string path);
    bool loadHdrTexture(std::string path);
    bool loadShader(std::string path);
    bool loadMesh(std::string path, bool createEntities = true);

    // TODO: Documentation
    bool processNode(aiNode* node, const aiScene* scene, std::string path, Transform* parent = nullptr);
    Entity* processBone(aiNode* node, const aiScene* scene, std::string path, Transform* parent = nullptr);
    Animation* processAnimations(const aiScene* scene, std::string path);
    Mesh* createMesh(aiMesh* node, std::string path);

    static inline glm::vec3 aiVectorToGlmVec3(const aiVector3D &v) { return glm::vec3(v.x, v.y, v.z); }
    static inline glm::vec2 aiVectorToGlmVec2(const aiVector3D &v) { return glm::vec2(v.x, v.y); }
    static inline glm::quat aiQuaternionToGlmQuat(const aiQuaternion &q) { return glm::quat(q.w, q.x, q.y, q.z); }
    static inline glm::mat4 aiMatrixToGlmMat4(const aiMatrix4x4 &m) { return glm::transpose(glm::make_mat4(&m.a1)); }
    static inline glm::mat4 aiMatrixToGlmMat4(const aiMatrix3x3 &m) { return glm::transpose(glm::make_mat3(&m.a1)); }
};

#endif // _RESOURCEMODULE_HPP

