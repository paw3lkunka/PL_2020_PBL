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

class ObjectModule;
class ObjectMaker;
class SceneModule;
class Bone;
class SceneWriter;
struct Transform;

/**
 * @brief Resource Module class for reading and storage assets data
 */
class AssetReader
{
    friend class ObjectMaker;
    friend class ObjectModule;
public:

    AssetReader(ObjectModule* objModule);
    ~AssetReader() = default;

    //TODO: Better solution for saving animation ticks
    std::unordered_map<std::string, double> animationTicks;

private:
    ObjectModule* objectModulePtr;

    /**
     * @brief assimp importer reference;
     */
    Assimp::Importer importer;

    static bool hasInstance;

    ///@brief var for process vertex attributes method
    glm::vec3 tempVector;
    ///@brief helpful while reading bones
    unsigned int bonesAmount = 0;
    ///@brief map os bones, key = name, value = index
    std::unordered_map<std::string, unsigned int> boneMapping;
    
    //Storages
    std::unordered_map<std::string, AudioFile> audioClips;
    std::unordered_map<std::string, TextureData> textures;
    std::unordered_map<std::string, std::string> shaders;


    //load files to storages methods
    bool loadAudioClip(std::string path);
    bool loadTexture(std::string path);
    bool loadShader(std::string path);
    bool loadMesh(std::string path);
    bool loadSkinnedMesh(std::string path);

    /**
     * @brief Recursive method of processing meshes
     * 
     * @param node of imported model
     * @param scene assimp structure containing nodes
     * @param path path to file to load (for saving to map purposes)
     * @return true meshes processed and saved to map
     * @return false something went wrong during processing meshes
     */
    bool processMeshNode(aiNode* node, const aiScene* scene, std::string path);

    /**
     * @brief Recursive method of processing meshes with bones
     * 
     * @param node of imported model
     * @param scene assimp structure containing nodes
     * @param path to file to load (for saving purposes)
     * @return true meshes with pones processed and saved
     * @return false something went wrong during processing
     */
    bool processSkinnedMeshNode(aiNode* node, const aiScene* scene, std::string path);

    /**
     * @brief Method that adds bone data to skinned vertex
     * 
     * @param vertex to add bone data
     * @param boneIndex index of the bone 
     * @param weight weight of the bone
     */
    void addBoneDataToVertex(VertexSkinned& vertex, unsigned int& boneIndex, float& weight);

    /**
     * @brief method that saves basic vertex attributes
     * 
     * @param vert reference to vertex to save values
     * @param mesh pointer to mesh in which that vertex is
     * @param vertexIndex index of vertex in mesh
     */
    void processVertexAttributes(Vertex& vert, aiMesh* mesh, int vertexIndex);

    /**
     * @brief process mesh indices
     * 
     * @param indices reference for vector to save indices
     * @param mesh pointer to mesh from indices are taken
     */
    void processIndices(std::vector<unsigned int>& indices, aiMesh* mesh);

    /**
     * @brief method that finds root node for bones
     * 
     * @param rootNode root node to search
     * @return aiNode* node which is root node for bones
     */
    aiNode* findRootBone(aiNode* rootNode);

    /**
     * @brief method that finds animation node associated with bone node
     * 
     * @param animPtr pointer to scene animation
     * @param nodeName node name for search
     * @return aiNodeAnim* animation node associated with bone node
     */
    aiNodeAnim* findNodeAnim(aiAnimation* animPtr, std::string nodeName);

    /**
     * @brief method that saves bones to entities
     * 
     * @param rootNode root node of bones
     * @param parent parent transform for next transform
     * @param scene pointer to scene for reference when finding animation node
     * @param path file path from bones are read
     * @return true bones are processed and saved to entities
     * @return false something went wrong during processing bones
     */
    bool processBones(aiNode* rootNode, Transform* parent, const aiScene* scene, std::string path);

    /**
     * @brief Helper function to cast assimp matrix4x4 to glm::mat4
     * 
     * @param matrix assimp matrix to cast
     * @return glm::mat4 casted matrix
     */
    glm::mat4 aiMatrixToGlmMat(aiMatrix4x4 matrix);

    /**
     * @brief copies position and rotation keys to map of AnimKeys
     * 
     * @param bone bone to process
     * @param animNode anim node to process
     */
    void copyToMap(Bone* bone, aiNodeAnim* animNode);

};

#endif // _RESOURCEMODULE_HPP

