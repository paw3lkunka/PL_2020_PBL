#include "AssetReader.hpp"
#include "FileStructures.inl"
#include "Core.hpp"
#include "mesh/MeshCustom.hpp"
#include "Message.inl"
#include "Transform.hh"
#include "Bone.inl"
#include "Skeleton.inl"
#include "mesh/MeshDataStructures.inl"
#include "ObjectMaker.hpp"
#include "ObjectModule.hpp"
#include "ObjectExceptions.inl"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <fstream>
#include <sstream>
#include <list>
#include <vector>
#include <map>
#include <string>

#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

#include <glad/glad.h>

bool AssetReader::hasInstance = false;
unsigned int AssetReader::bonesAmount = 0;

AssetReader::AssetReader(ObjectModule* objModule) : objectModulePtr(objModule) 
{
    if(hasInstance)
    {
        throw TooManyInstancesException("Asset Reader");
    }
    hasInstance = true;
}

#pragma region Loaders

AudioFile* AssetReader::loadAudioClip(std::string path)
{
    AudioFile audioData;
    if(audioData.load(path))
    {
        audioClips.insert( std::pair(path, audioData) );
    }

    std::unordered_map<std::string, AudioFile>::iterator iter = audioClips.find(path);
    if(iter == audioClips.end())
    {
        return nullptr;
    }

    return &iter->second;
}

bool AssetReader::loadTexture(std::string path)
{
    TextureData tData;
    tData.data = stbi_load(path.c_str(), &tData.width, &tData.height, &tData.nrComponents, 0);
    if(tData.width != 0 && tData.height != 0 && tData.nrComponents != 0)
    {
        textures.insert( std::pair(path, tData) );
        
        std::unordered_map<std::string, TextureData>::iterator iter = textures.find(path);
        std::cout << "Tex loaded: " << path << std::endl;
        return iter != textures.end();
    }
    else
    {
        std::cerr << "Error while loading texture " << path << std::endl;
    }
    return false;
}

bool AssetReader::loadHdrTexture(std::string path)
{
    TextureHdrData tData;
    tData.data = stbi_loadf(path.c_str(), &tData.width, &tData.height, &tData.nrComponents, 0);
    if(tData.width != 0 && tData.height != 0 && tData.nrComponents != 0)
    {
        texturesHdr.insert( std::pair(path, tData) );
        
        std::unordered_map<std::string, TextureHdrData>::iterator iter = texturesHdr.find(path);
        std::cout << "Tex hdr loaded: " << path << std::endl;
        return iter != texturesHdr.end();
    }
    else
    {
        std::cerr << "Error while loading hdr texture " << path << std::endl;
    }
    return false;
}

bool AssetReader::loadShader(std::string path)
{
    std::string buffer = "";
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        file.open(path);
        std::stringstream stream;
        stream << file.rdbuf();
        file.close();
        buffer = stream.str();
    }
    catch(std::ifstream::failure e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << "Error while loading " << path << std::endl;
        return false;
    }

    shaders.insert( std::pair(path, buffer));
    std::unordered_map<std::string, std::string>::iterator iter = shaders.find(path);

    return iter != shaders.end();
}


bool AssetReader::loadMesh(std::string path, bool customName, bool createEntities)
{
    makeEntities = createEntities;

    // ? +++++ Load the assimp scene from provided path +++++
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals | aiProcess_GenBoundingBoxes | aiProcess_LimitBoneWeights);
    
    // Check for assimp errors and display them
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "Assimp Error: " << importer.GetErrorString();
        return false;
    }
    else
    {
        globalInverseTransform = glm::inverse(aiMatrixToGlmMat4(scene->mRootNode->mTransformation));

        // * ----- Process and create entities with mesh renderers -----
        processNode(scene->mRootNode, scene, path, customName);

        // * ----- Process and create bones -----
        Entity* rootBone = processBone(scene->mRootNode, scene, path);

        if(rootBone != nullptr)
        {
            // * ----- Process animations -----
            Animation* animation = processAnimations(scene, path);
            // * ----- Create skeleton object and bind root node -----
            std::size_t foundIndex = rootBone->getComponentPtr<Bone>()->boneName.find_last_of("/\\");
            objectModulePtr->newEntity(1, rootBone->getComponentPtr<Bone>()->boneName.substr(foundIndex + 1) + std::string("_skeleton"));
            auto s = objectModulePtr->newEmptyComponentForLastEntity<Skeleton>();
            s->animation = animation;
            s->globalInverseTransform = globalInverseTransform;
            s->rootBone = rootBone;
        }   
    }

    makeEntities = true;
    return true;
}

bool AssetReader::processNode(aiNode* node, const aiScene* scene, std::string path, bool customName, Transform* parent)
{
    Transform* nodeTransform = nullptr;
    // ? +++++ Process meshes (if any) ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    for (size_t i = 0; i < node->mNumMeshes; i++)
    {
        std::string meshName = "aaaaa";
        if (customName)
        {
            meshName = "mesh" + std::to_string(i);
        }
        else
        {
            meshName = scene->mMeshes[node->mMeshes[i]]->mName.C_Str();
        }

        Mesh* newMesh = createMesh(scene->mMeshes[node->mMeshes[i]], path, meshName, customName);

        // * ----- Create new entity with provided name -----
        Entity* e = nullptr;
        if (makeEntities)
        {
            size_t index = path.find_last_of("/\\");

            e = objectModulePtr->newEntity(2, path.substr(index + 1) + "/" + meshName);
            std::cout << "Mesh entity:" << path.substr(index + 1) + "/" + meshName << std::endl;

            // * ----- Create transform and set parent -----
            nodeTransform = objectModulePtr->newEmptyComponentForLastEntity<Transform>();
            
            // HACK: Decomposing matrix and setting values individually
            glm::mat4 localTransformMat = aiMatrixToGlmMat4(node->mTransformation);
            glm::vec3 position, scale, skew;
            glm::vec4 perspective;
            glm::quat rotation;
            glm::decompose(localTransformMat, scale, rotation, position, skew, perspective);

            nodeTransform->getLocalPositionModifiable() = position;
            nodeTransform->getLocalRotationModifiable() = rotation;
            nodeTransform->getLocalScaleModifiable() = scale;
            
            if (parent != nullptr)
            {
                nodeTransform->setParent(parent);
            }
            else
            {
                nodeTransform->setParent(&GetCore().sceneModule.rootNode);
            }

            std::string dirPath = path.substr(0, path.find_last_of("/\\"));
            dirPath = dirPath.substr(dirPath.find_last_of("/\\") + 1);

            // HACK: Making a terrain renderer based on folder name is basically a hack
            if (dirPath == "Terrain")
            {
                auto tr = objectModulePtr->newEmptyComponent<TerrainRenderer>();
                    tr->terrainMesh = newMesh;
                e->addComponent(tr);
            }
            else
            {
                // * ----- Create mesh renderer and add created mesh -----
                auto mr = objectModulePtr->newEmptyComponent<MeshRenderer>();
                    mr->mesh = newMesh;
                e->addComponent(mr);
            }
        }
    }

    if (parent == nullptr && makeEntities && scene->mNumMeshes > 1)
    {
        size_t index = path.find_last_of("/\\");
        Entity* e = objectModulePtr->newEntity(1, path.substr(index + 1));
        if(e->getComponentPtr<Transform>() == nullptr)
        {
            nodeTransform = objectModulePtr->newEmptyComponent<Transform>();
            nodeTransform->setParent(&GetCore().sceneModule.rootNode);
            e->addComponent(nodeTransform);
        }
    }

    // ? +++++ Recursively call process node for all the children nodes +++++++++++++++++++++++
    for (int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene, path, customName, nodeTransform);
    }

    return true;
}

Entity* AssetReader::processBone(aiNode* node, const aiScene* scene, std::string path, Transform* parent)
{
    // ? +++++ Check if node is considered a bone ++++++++++++++++++++++++++++++++++++++++++++++++
    std::string filename = path.substr(path.find_last_of("\\/") + 1);
    Entity* e = objectModulePtr->getEntityPtrByName((filename + "/" + node->mName.C_Str() + "(bone)").c_str());

    if (e != nullptr)
    {
        Transform* t = objectModulePtr->newEmptyComponent<Transform>();
        
        // * ----- If bone of the same name as node is found, fill additional bone data -----
        glm::quat rotation;
        glm::vec3 position, scale, shit;
        glm::vec4 shit4;
        glm::decompose(aiMatrixToGlmMat4(node->mTransformation), scale, rotation, position, shit, shit4);
        t->getLocalPositionModifiable() = position;
        t->getLocalRotationModifiable() = rotation;
        t->getLocalScaleModifiable() = scale;
        if(parent != nullptr)
        {
            t->setParent(parent);
        }
        else
        {
            t->setParent(&GetCore().sceneModule.rootNode);
        }
        e->addComponent(t);
        
        std::cout << "Bone: " << filename + "/" +  node->mName.C_Str() << std::endl;
        // ? +++++ Recursively call process node for all the children nodes +++++++++++++++++++++++
        for (unsigned int i = 0; i < node->mNumChildren; ++i)
        {
            processBone(node->mChildren[i], scene, path, t);
        }
        return e;
    }
    else
    {
        // * ----- Otherwise return nullptr -----
        Entity* rootBone = nullptr;
        for (unsigned int i = 0; i < node->mNumChildren; ++i)
        {
            Entity* bonePtr = processBone(node->mChildren[i], scene, path, nullptr);
            if (bonePtr != nullptr)
            {
                rootBone = bonePtr;
            }
        }
        return rootBone;
    }
    return nullptr;
}

Animation* AssetReader::processAnimations(const aiScene* scene, std::string path)
{
    if (scene->HasAnimations())
    {
        Animation* firstAnim = nullptr;
        for (size_t i = 0; i < scene->mNumAnimations; i++)
        {
            Animation newAnim;
            
            for (size_t j = 0; j < scene->mAnimations[i]->mNumChannels; j++)
            {
                std::string filename = path.substr(path.find_last_of("\\/") + 1);
                unsigned int nodeBoneID = objectModulePtr->getEntityPtrByName((filename + "/" + scene->mAnimations[i]->mChannels[j]->mNodeName.C_Str() + "(bone)").c_str())->getComponentPtr<Bone>()->boneID;
                newAnim.addNode(nodeBoneID);
                newAnim.findAnimNode(nodeBoneID)->animationLength = scene->mAnimations[i]->mDuration;

                for (size_t k = 0; k < scene->mAnimations[i]->mChannels[j]->mNumPositionKeys; k++)
                {
                    newAnim.addPositionKey(nodeBoneID, 
                        scene->mAnimations[i]->mChannels[j]->mPositionKeys[k].mTime,
                        aiVectorToGlmVec3(scene->mAnimations[i]->mChannels[j]->mPositionKeys[k].mValue));
                }
                
                for (size_t k = 0; k < scene->mAnimations[i]->mChannels[j]->mNumRotationKeys; k++)
                {
                    newAnim.addRotationKey(nodeBoneID, 
                        scene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mTime,
                        aiQuaternionToGlmQuat(scene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mValue));
                }

                for (size_t k = 0; k < scene->mAnimations[i]->mChannels[j]->mNumScalingKeys; k++)
                {
                    newAnim.addScaleKey(nodeBoneID, 
                        scene->mAnimations[i]->mChannels[j]->mScalingKeys[k].mTime,
                        aiVectorToGlmVec3(scene->mAnimations[i]->mChannels[j]->mScalingKeys[k].mValue));
                }
            }

            Animation* tempAnim = objectModulePtr->objectMaker.newAnimation(newAnim, path, scene->mAnimations[i]->mName.C_Str());
            if (i == 0)
            {
                firstAnim = tempAnim;
            }
        }
        
        return firstAnim;
    }
    else
    {
        return nullptr;
    }
}

Mesh* AssetReader::createMesh(aiMesh* mesh, std::string path, std::string meshName, bool customName)
{
    std::list<glm::vec3> positions;
    std::list<glm::vec3> normals;
    std::list<glm::vec3> tangents;
    std::list<glm::vec2> texcoords;
    struct VertexWeightInfo
    {
        float weight;
        unsigned int boneId;
    };
    std::multimap<unsigned int, VertexWeightInfo> idWeight;

    // ? +++++ Loading data for base vertex +++++
    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        // * ----- Base data: position, normal, tangent -----
        positions.push_back( {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z} );
        normals.push_back( {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z} );
        tangents.push_back( {mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z} );

        // * ----- Optional data: texcoords -----
        // TODO: Support more than one uv channel
        if (mesh->mTextureCoords[0])
        {
            texcoords.push_back( {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y} );
        }
        else
        {
            texcoords.push_back( {0.0f, 0.0f} );
        }
    }

    std::string filename = path.substr(path.find_last_of("\\/") + 1);
    // ? ----- Loading optional data: bones, boneWeights, boneIDs -----
    for (size_t i = 0; i < mesh->mNumBones; i++)
    {
        // * Author note: each loop inserts one bone into global map of bones and then saves all the weights and bone ids
        // * by vertex id to local map. The bone id is created from the size of the map to ensure uniqueness of the id,
        // * because this is the only way to map vertex to bone.
        unsigned int globalBoneIndex = bonesAmount;

        // XXX: I'm not sure if the insert will just do nothing if the key already exists (and that's what I want it to do) 
        // ! All bone names follow convention path/boneName to ensure uniqueness between diferrent models   
        Entity* e = objectModulePtr->getEntityPtrByName((filename + "/" + mesh->mBones[i]->mName.C_Str() + "(bone)").c_str() );
        if (e == nullptr)
        {
            Entity* ent = objectModulePtr->newEntity(2, filename + "/" + mesh->mBones[i]->mName.C_Str() + "(bone)");
            Bone* bone = objectModulePtr->newEmptyComponentForLastEntity<Bone>();
                bone->boneID = globalBoneIndex;
                bone->boneName = filename + "/" + mesh->mBones[i]->mName.C_Str();
                bone->offsetMatrix = aiMatrixToGlmMat4(mesh->mBones[i]->mOffsetMatrix);
            bonesAmount++;
        }
        else
        {
            globalBoneIndex = e->getComponentPtr<Bone>()->boneID;
        }
        

        for (size_t j = 0; j < mesh->mBones[i]->mNumWeights; j++)
        {
            idWeight.insert(
                { 
                    mesh->mBones[i]->mWeights[j].mVertexId,
                    VertexWeightInfo 
                    {
                        mesh->mBones[i]->mWeights[j].mWeight, // weight
                        globalBoneIndex // bone id
                    }
                }
            );
        }
    }

    // ? +++++ Loading indices data +++++
    std::vector<unsigned int> indices;
    for (size_t i = 0; i < mesh->mNumFaces; i++)
    {
        for (size_t j = 0; j < mesh->mFaces[i].mNumIndices; j++)
        {
            indices.push_back(mesh->mFaces[i].mIndices[j]);
        }
    }

    // TODO: A lot of code is repeated here, there might be a way to make it simpler but now I don't have the patience to bother
    // ? +++++ Check if mesh is influenced by bones +++++
    if (!mesh->HasBones())
    {
        // ? +++++ Create regular mesh and return it +++++
        std::vector<Vertex> vertices;

        std::list<glm::vec3>::iterator posIter = positions.begin();
        std::list<glm::vec3>::iterator normIter = normals.begin();
        std::list<glm::vec3>::iterator tangIter = tangents.begin();
        std::list<glm::vec2>::iterator uvIter = texcoords.begin();
        while (posIter != positions.end())
        {
            vertices.push_back(
                Vertex {
                    *posIter,
                    *normIter,
                    *tangIter,
                    *uvIter
                }
            );

            posIter++;
            normIter++;
            tangIter++;
            uvIter++;
        }

        // ? +++++ Create mesh bounds from AABB +++++
        Bounds bounds;
        bounds.maxBound = {mesh->mAABB.mMax.x, mesh->mAABB.mMax.y, mesh->mAABB.mMax.z};
        bounds.minBound = {mesh->mAABB.mMin.x, mesh->mAABB.mMin.y, mesh->mAABB.mMin.z};

        // * ----- Add mesh to resource container -----
        // ! ===== Return appropriate mesh pointer =====
        return objectModulePtr->objectMaker.newMesh<MeshCustom, Vertex>(vertices, indices, bounds, path, meshName, customName);
    }
    else
    {
        // ? +++++ Create skinned mesh and return it +++++
        std::vector<VertexSkinned> vertices;
        
        std::list<glm::vec3>::iterator posIter = positions.begin();
        std::list<glm::vec3>::iterator normIter = normals.begin();
        std::list<glm::vec3>::iterator tangIter = tangents.begin();
        std::list<glm::vec2>::iterator uvIter = texcoords.begin();
        unsigned int i = 0;
        while (posIter != positions.end())
        {
            // * ----- Getting by the vertex id -----
            auto range = idWeight.equal_range(i);
            
            int count = std::distance(range.first, range.second);
            if (count > VertexSkinned::MAX_WEIGHTS)
            {
                // TODO: Proper error logging
                std::cerr << "Tried to insert " << count << " weights while max is " << VertexSkinned::MAX_WEIGHTS << "\n";
                std::cerr << "The excess will be clipped but the weight values will be incorrect, either increase the max weights or decrease bone influence in the model\n";
            }

            unsigned int ids[VertexSkinned::MAX_WEIGHTS];
            float weights[VertexSkinned::MAX_WEIGHTS];

            int number = VertexSkinned::MAX_WEIGHTS < count ? VertexSkinned::MAX_WEIGHTS : count; 
            for (size_t j = 0; j < 4; j++)
            {
                if (j < number)
                {
                    ids[j] = range.first->second.boneId;
                    weights[j] = range.first->second.weight;
                }
                else
                {
                    ids[j] = 0;
                    weights[j] = 0.0;
                }
                // Increment iterator
                range.first++;
            }
            

            vertices.push_back(
                VertexSkinned {
                    *posIter,
                    *normIter,
                    *tangIter,
                    *uvIter,
                    {ids[0], ids[1], ids[2], ids[3]},
                    {weights[0], weights[1], weights[2], weights[3]}
                }
            );

            posIter++;
            normIter++;
            tangIter++;
            uvIter++;
            i++;
        }

        // ? ----- Create mesh bounds from AABB -----
        Bounds bounds;
        bounds.maxBound = {mesh->mAABB.mMax.x, mesh->mAABB.mMax.y, mesh->mAABB.mMax.z};
        bounds.minBound = {mesh->mAABB.mMin.x, mesh->mAABB.mMin.y, mesh->mAABB.mMin.z};

        // * ----- Add mesh to resource container -----
        // ! ===== Return appropriate mesh pointer =====
        return objectModulePtr->objectMaker.newMesh<MeshSkinned, VertexSkinned>(vertices, indices, bounds, path, meshName, customName);
    }
}

#pragma endregion

#pragma endregion

#pragma region Helpers

void displayNodeHierarchy(aiNode* node, int depth)
{
    for (size_t i = 0; i < depth; i++)
    {
        std::cout << '-';
    }
    std::cout << node->mName.C_Str() << '\n';

    for (size_t i = 0; i < node->mNumChildren; i++)
    {
        displayNodeHierarchy(node->mChildren[i], depth + 1);
    }
}

void displayGlmMat4Decomposed(glm::mat4 mat)
{
    glm::vec3 position, scale, skew;
    glm::vec4 perspective;
    glm::quat rotation;
    //TODO, can we omit decompose?
    glm::decompose(mat, scale, rotation, position, skew, perspective);
    glm::vec3 euler = glm::degrees(glm::eulerAngles(rotation));
    std::cout << "Position: " << position.x << ' ' << position.y << ' ' << position.z << '\n';
    std::cout << "Rotation (deg): " << euler.x << ' ' << euler.y << ' ' << euler.z << '\n';
    std::cout << "Scale: " << scale.x << ' ' << scale.y << ' ' << scale.z << '\n';
}

void displayGlmVec3(glm::vec3 vec)
{
    std::cout << vec.x << ' ' << vec.y << ' ' << vec.z << '\n';
}

void displayAssimpMat4(aiMatrix4x4 mat)
{
    std::cout << mat[0][0] << '\t' << mat[0][1] << '\t' << mat[0][2] << '\t' << mat[0][3] << '\n';
    std::cout << mat[1][0] << '\t' << mat[1][1] << '\t' << mat[1][2] << '\t' << mat[1][3] << '\n'; 
    std::cout << mat[2][0] << '\t' << mat[2][1] << '\t' << mat[2][2] << '\t' << mat[2][3] << '\n'; 
    std::cout << mat[3][0] << '\t' << mat[3][1] << '\t' << mat[3][2] << '\t' << mat[3][3] << '\n'; 
}

void displayGlmMat4(glm::mat4 mat)
{
    std::cout << mat[0][0] << '\t' << mat[1][0] << '\t' << mat[2][0] << '\t' << mat[3][0] << '\n';
    std::cout << mat[0][1] << '\t' << mat[1][1] << '\t' << mat[2][1] << '\t' << mat[3][1] << '\n'; 
    std::cout << mat[0][2] << '\t' << mat[1][2] << '\t' << mat[2][2] << '\t' << mat[3][2] << '\n'; 
    std::cout << mat[0][3] << '\t' << mat[1][3] << '\t' << mat[2][3] << '\t' << mat[3][3] << '\n'; 
}

#pragma endregion