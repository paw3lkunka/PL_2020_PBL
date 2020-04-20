#ifndef _RESOURCEMODULE_HPP
#define _RESOURCEMODULE_HPP

#include "IModule.inl"
#include "AudioFile.h"

#include <unordered_map>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <vector>
#include <glm/mat4x4.hpp>

class Message;
class Animation;
class Mesh;
struct TextureData;

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

    // TODO: set these to private later (public just for testing)
    std::unordered_map<std::string, AudioFile<unsigned char> > audioClips;
    std::unordered_map<std::string, TextureData> textures;
    std::unordered_map<std::string, Mesh> meshes;
    std::unordered_map<std::string, std::string> shaders;
    std::unordered_map<std::string, Animation> animations;

private:

    /**
     * @brief assimp importer reference;
     */
    Assimp::Importer importer;

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
    void addBoneDataToVertex(SkinnedVertex& vertex, unsigned int& boneIndex, float& weight);

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
     * @brief Helper function to cast assimp matrix4x4 to glm::mat4
     * 
     * @param matrix assimp matrix to cast
     * @return glm::mat4 casted matrix
     */
    glm::mat4 aiMatrixToGlmMat(aiMatrix4x4 matrix);

};

#endif // _RESOURCEMODULE_HPP

