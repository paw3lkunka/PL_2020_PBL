#ifndef _RESOURCEMODULE_HPP
#define _RESOURCEMODULE_HPP

#include "IModule.inl"
#include "AudioFile.hpp"
#include "AssetStructers.inl"
#include "MeshSkinned.hpp"
#include "MeshCustom.hpp"

#include <unordered_map>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <vector>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Message;
class ObjectModule;
class Bone;
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
    //TODO: Better solution for saving animation ticks
    std::unordered_map<std::string, double> animationTicks;
    /**
     * @brief Bones data collection
     * @key path/bone_name
     */
    std::unordered_map<std::string, BoneInfo> bones;
    std::vector<glm::mat4> finalTransforms;

private:

    /**
     * @brief assimp importer reference;
     */
    Assimp::Importer importer;

    //HACK TEMPORARY HACK, I AM TOO TIRED TO DO IT BETTER
    glm::vec3 tempVector;
    
    //Storages

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
    bool processBone(aiNode* node, const aiScene* scene, std::string path, glm::mat4 parentTransform = glm::mat4(1.0f));
    Mesh* createMesh(aiMesh* node, std::string path);

    static inline glm::vec3 aiVectortoGlmVec3(const aiVector3D &v) { return glm::vec3(v.x, v.y, v.z); }
    static inline glm::vec2 aiVectortoGlmVec2(const aiVector3D &v) { return glm::vec2(v.x, v.y); }
    static inline glm::quat aiQuaternionToGlmQuat(const aiQuaternion &q) { return glm::quat(q.w, q.x, q.y, q.z); }
    static inline glm::mat4 aiMatrixToGlmMat4(const aiMatrix4x4 &m) { return glm::transpose(glm::make_mat4(&m.a1)); }
    static inline glm::mat4 aiMatrixToGlmMat4(const aiMatrix3x3 &m) { return glm::transpose(glm::make_mat3(&m.a1)); }
};

#endif // _RESOURCEMODULE_HPP

