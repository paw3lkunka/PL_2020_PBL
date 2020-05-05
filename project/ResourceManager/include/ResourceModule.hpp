#ifndef _RESOURCEMODULE_HPP
#define _RESOURCEMODULE_HPP

#include "IModule.inl"
#include "AudioFile.hpp"
#include "AssetStructers.inl"
#include "MeshSkinned.hpp"
#include "MeshCustom.hpp"
#include "Animation.hpp"
#include "Bone.inl"

#include <unordered_map>
#include <map>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <vector>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Message;
class ObjectModule;
struct Transform;

void displayNodeHierarchy(aiNode* node, int depth = 0);
void displayGlmVec3(glm::vec3 vec);
void displayGlmMat4Decomposed(glm::mat4 mat);
void displayAssimpMat4(aiMatrix4x4 mat);
void displayGlmMat4(glm::mat4 mat);

/**
 * @brief Resource Module class for reading and storage assets data
 */
class ResourceModule : public IModule
{
public:
    /**
     * @brief inherited from IModule
     * 
     * @param msg message received
     */
    void receiveMessage(Message msg);

    ///HACK: Connection between Resource Module and Object Module
    ObjectModule* objectModulePtr;

    //TODO: Move STORAGES to private
    //Storages
    std::unordered_map<std::string, AudioFile> audioClips;
    std::unordered_map<std::string, TextureData> textures;
    std::unordered_map<std::string, MeshSkinned> skinnedMeshes;
    std::unordered_map<std::string, MeshCustom> meshes;
    std::unordered_map<std::string, std::string> shaders;
    /**
     * @brief Bones data collection
     * @key path/bone_name
     */
    std::unordered_map<std::string, Bone> bones;
    std::map<std::string, Animation> animations;
    std::vector<glm::mat4> finalTransforms;

private:

    /**
     * @brief assimp importer reference;
     */
    Assimp::Importer importer;

    //Storages

    // FIXME: I dont have the patience to do it in other way
    glm::mat4 globalInverseTransform;

    // Send data to MessageBus methods
    bool sendAudioClip(std::string path);
    bool sendTexture(std::string path);
    bool sendMesh(std::string path);
    bool sendShader(std::string path);
    bool sendSkinnedMesh(std::string path);

    //load files to storages methods
    bool loadAudioClip(std::string path);
    bool loadTexture(std::string path);
    bool loadShader(std::string path);
    bool loadMesh(std::string path);

    // TODO: Documentation
    bool processNode(aiNode* node, const aiScene* scene, std::string path, Transform* parent = nullptr);
    Bone* processBone(aiNode* node, const aiScene* scene, std::string path, Bone* parent = nullptr);
    bool processAnimations(const aiScene* scene, std::string path);
    Mesh* createMesh(aiMesh* node, std::string path);

    static inline glm::vec3 aiVectortoGlmVec3(const aiVector3D &v) { return glm::vec3(v.x, v.y, v.z); }
    static inline glm::vec2 aiVectortoGlmVec2(const aiVector3D &v) { return glm::vec2(v.x, v.y); }
    static inline glm::quat aiQuaternionToGlmQuat(const aiQuaternion &q) { return glm::quat(q.w, q.x, q.y, q.z); }
    static inline glm::mat4 aiMatrixToGlmMat4(const aiMatrix4x4 &m) { return glm::transpose(glm::make_mat4(&m.a1)); }
    static inline glm::mat4 aiMatrixToGlmMat4(const aiMatrix3x3 &m) { return glm::transpose(glm::make_mat3(&m.a1)); }
};

#endif // _RESOURCEMODULE_HPP

