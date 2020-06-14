#include "TerrainUtils.hpp"

#include "Core.hpp"
#include <fstream>
#include <map>

#include <glm/gtx/string_cast.hpp>

void TerrainUtils::loadAllTerrainChunks()
{
    auto terrainShader = GetCore().objectModule.newShader("terrainShader", "Resources/Shaders/StandardTerrain/StandardTerrain.vert", "Resources/Shaders/StandardTerrain/StandardTerrain.frag");

    TextureCreateInfo texCreateInfo = {};
    texCreateInfo.generateMipmaps = true;
    texCreateInfo.magFilter = GL_LINEAR;
    texCreateInfo.minFilter = GL_LINEAR_MIPMAP_LINEAR;
    texCreateInfo.wrapMode = GL_REPEAT;

    auto grass1Diffuse = GetCore().objectModule.newTexture("Resources/Textures/grassRock/aerial_grass_rock_diff_2k.png", texCreateInfo);
    auto grass1Normal = GetCore().objectModule.newTexture("Resources/Textures/grassRock/aerial_grass_rock_nor_2k.png", texCreateInfo);
    auto grass1Specular = GetCore().objectModule.newTexture("Resources/Textures/grassRock/aerial_grass_rock_OccRoughMet_2k.png", texCreateInfo);

    auto gravel1Diffuse = GetCore().objectModule.newTexture("Resources/Textures/rockGround/rock_ground_02_diff_2k.png", texCreateInfo);
    auto gravel1Normal = GetCore().objectModule.newTexture("Resources/Textures/rockGround/rock_ground_02_nor_2k.png", texCreateInfo);
    auto gravel1Specular = GetCore().objectModule.newTexture("Resources/Textures/rockGround/rock_ground_02_occRouMet.png", texCreateInfo);

    auto rockGrassDiffuse = GetCore().objectModule.newTexture("Resources/Textures/rockGrass/rockGrass_diffuse.png", texCreateInfo);
    auto rockGrassNormal = GetCore().objectModule.newTexture("Resources/Textures/rockGrass/rockGrass_normal.png", texCreateInfo);

    auto whiteTex = GetCore().objectModule.newTexture("Resources/Textures/white.png", texCreateInfo);
    auto blackTex = GetCore().objectModule.newTexture("Resources/Textures/black.png", texCreateInfo);
    auto bumpTex = GetCore().objectModule.newTexture("Resources/Textures/bump.png", texCreateInfo);
    auto occroumetTex = GetCore().objectModule.newTexture("Resources/Textures/occroumet.png", texCreateInfo);

    Material* terrainMat = GetCore().objectModule.newMaterial(terrainShader, "terrainMat", RenderType::Opaque);
    glm::vec2 uv0s = {2.0f, 2.0f};
    terrainMat->setVec2("uv0scale", uv0s);
    terrainMat->setTexture("diffuse0", rockGrassDiffuse);
    terrainMat->setTexture("normal0", rockGrassNormal);
    terrainMat->setTexture("orm0", occroumetTex);
    glm::vec2 uv1s = {10.0f, 10.0f};
    terrainMat->setVec2("uv1scale", uv1s);
    terrainMat->setTexture("diffuse1", gravel1Diffuse);
    terrainMat->setTexture("normal1", gravel1Normal);
    terrainMat->setTexture("orm1", gravel1Specular);
    glm::vec2 uv2s = {4.0f, 4.0f};
    terrainMat->setVec2("uv2scale", uv2s);
    terrainMat->setTexture("diffuse2", grass1Diffuse);
    terrainMat->setTexture("normal2", grass1Normal);
    terrainMat->setTexture("orm2", grass1Specular);
    glm::vec2 uv3s = {1.0f, 1.0f};
    terrainMat->setVec2("uv3scale", uv3s);
    terrainMat->setTexture("diffuse3", whiteTex);
    terrainMat->setTexture("normal3", bumpTex);
    terrainMat->setTexture("orm3", occroumetTex);

    Entity* terrain = GetCore().objectModule.newEntity(1, "Terrain");
    auto terrainRoot = GetCore().objectModule.newEmptyComponentForLastEntity<Transform>();
    terrainRoot->getLocalPositionModifiable() = glm::vec3(50, -100, -50);
    terrainRoot->setParent(&GetCore().sceneModule.rootNode);

    namespace fs = std::filesystem;
    std::string path = "Resources/Terrain/";
    for (auto& entry : fs::directory_iterator(path))
    {
        GetCore().objectModule.newModel(entry.path().string().c_str());
        std::string filename = entry.path().filename().string();
        Entity* entity = GetCore().objectModule.getEntityPtrByName((filename + "/mesh0").c_str());
        {
            size_t start = filename.find_first_of('(') + 1;
            size_t end = filename.find_first_of(')');
            size_t size = end - start;
            std::string parsePos = filename.substr(start, size);
            std::string delimiter = ", ";
            float positions[3];
            int i = 0;
            size_t pos;
            std::string token;
            while ((pos = parsePos.find(delimiter)) != std::string::npos)
            {
                token = parsePos.substr(0, pos);
                positions[i] = std::stof(token);
                parsePos.erase(0, pos + delimiter.length());
                i++;
            }
            positions[i] = std::stof(parsePos);
            
            auto t = entity->getComponentPtr<Transform>();
            t->getLocalPositionModifiable() = {-positions[0], positions[1], positions[2]};
            t->setParent(terrainRoot);

            auto mr = entity->getComponentPtr<TerrainRenderer>();
                mr->material = GetCore().objectModule.getMaterialPtrByName("terrainMat");
        }
    }
    path = "Resources/Splatmaps/";
    for(auto& entry : fs::directory_iterator(path))
    {
        TextureCreateInfo ti = {};
        ti.generateMipmaps = true;
        ti.magFilter = GL_LINEAR;
        ti.minFilter = GL_LINEAR_MIPMAP_LINEAR;
        ti.wrapMode = GL_CLAMP_TO_EDGE;

        Texture* splatTexture = GetCore().objectModule.newTexture(entry.path().string().c_str(), ti);
        std::string filename = entry.path().filename().string();
        filename = filename.substr(0, filename.find_last_of('.'));
        Entity* entity = GetCore().objectModule.getEntityPtrByName((filename +  ".obj/mesh0").c_str());
        {
            auto tr = entity->getComponentPtr<TerrainRenderer>();
            tr->splatmap = splatTexture;
        }
    }
}

void TerrainUtils::loadExportedUnityModels()
{
    namespace fs = std::filesystem;
    std::string path = "Resources/Models/Unity/";

    std::filesystem::directory_entry exportInfoPath;

    for(auto& entry : fs::directory_iterator(path))
    {
        if (entry.path().filename().string() == "exportinfo.txt")
        {
            exportInfoPath = entry;
        }
        else
        {
            GetCore().objectModule.newModel(entry.path().string().c_str(), true);
        }
    }

    Entity* propsRoot = GetCore().objectModule.newEntity(1, "propsRoot");
    auto propsRootTransform = GetCore().objectModule.newEmptyComponentForLastEntity<Transform>();
        propsRootTransform->setParent(&GetCore().sceneModule.rootNode);
        propsRootTransform->getLocalPositionModifiable() = glm::vec3(50, -100, -50);

    std::fstream exportInfo;
    exportInfo.open(exportInfoPath.path().string().c_str(), std::ios::in);
    if (exportInfo.is_open())
    {
        std::map<std::string, int> loadedModels;
        std::string modelPath = "";
        glm::vec3 position;
        glm::quat rotation;
        glm::vec3 scale;

        std::string line;
        size_t index;
        while(std::getline(exportInfo, line))
        {
            index = line.find_first_of('$');
            if (index != std::string::npos)
            {
                modelPath = line.substr(line.find_first_of(':') + 1);
                std::getline(exportInfo, line);
                position = stringToVec3(line);
                position.x = -position.x;
                std::getline(exportInfo, line);
                rotation = stringToQuat(line);
                std::getline(exportInfo, line);
                scale = stringToVec3(line);
                std::cout << "Position: " << glm::to_string(position) << '\n';
                std::cout << "Rotation: " << glm::to_string(rotation) << '\n';
                std::cout << "Scale: " << glm::to_string(scale) << '\n';

                std::string modelName = modelPath.substr(modelPath.find_last_of("/\\") + 1);
                Entity* existingModel = GetCore().objectModule.getEntityPtrByName(modelName.c_str());
                if (existingModel == nullptr)
                {
                    existingModel = GetCore().objectModule.getEntityPtrByName((modelName + "/mesh0").c_str());
                    if (existingModel == nullptr)
                    {
                        std::cerr << "Requested model not found, aborting.\n";
                        return;
                    }
                }

                auto modelInfo = loadedModels.find(std::string(modelName));
                if (modelInfo != loadedModels.end())
                {
                    modelInfo->second++;
                    Entity* rootEntity = GetCore().objectModule.newEntity(2, modelName + " " + std::to_string(modelInfo->second));
                        auto rootTransform = GetCore().objectModule.newEmptyComponentForLastEntity<Transform>();
                            rootTransform->getLocalPositionModifiable() = position;
                            rootTransform->getLocalRotationModifiable() = rotation;
                            rootTransform->getLocalScaleModifiable() = scale;
                            rootTransform->setParent(propsRootTransform);
                            //rootTransform->setParent(GetCore().objectModule.getEntityPtrByName("Terrain")->getComponentPtr<Transform>());
                            //rootTransform->setParent(&GetCore().sceneModule.rootNode);

                    int entitiesToCreate = existingModel->getComponentPtr<Transform>()->children.size();
                    if (entitiesToCreate > 1)
                    {
                        auto children = existingModel->getComponentPtr<Transform>()->children;
                        for(auto child : children)
                        {
                            Entity* submeshEntity = GetCore().objectModule.newEntity(2, child->entityPtr->getName() + " " + std::to_string(modelInfo->second));
                            auto t = GetCore().objectModule.newEmptyComponentForLastEntity<Transform>();
                                t->setParent(rootTransform);
                            auto mr = GetCore().objectModule.newEmptyComponentForLastEntity<MeshRenderer>();
                                mr->mesh = child->entityPtr->getComponentPtr<MeshRenderer>()->mesh;
                                mr->material = child->entityPtr->getComponentPtr<MeshRenderer>()->material;
                        }
                    }
                    else
                    {
                        auto mr = GetCore().objectModule.newEmptyComponentForLastEntity<MeshRenderer>();
                            mr->mesh = existingModel->getComponentPtr<MeshRenderer>()->mesh;
                            mr->material = existingModel->getComponentPtr<MeshRenderer>()->material;
                    }
                }
                else
                {
                    loadedModels.insert({modelName, 1});
                    auto t = existingModel->getComponentPtr<Transform>();
                        t->getLocalPositionModifiable() = position;
                        t->getLocalRotationModifiable() = rotation;
                        t->getLocalScaleModifiable() = scale;
                        t->setParent(propsRootTransform);
                        //t->setParent(GetCore().objectModule.getEntityPtrByName("Terrain")->getComponentPtr<Transform>());
                        //t->setParent(&GetCore().sceneModule.rootNode);
                }
                
            }
            
        }
    }
    else
    {
        std::cerr << "exportinfo.txt file not found!\n";
    }
}

glm::vec3 TerrainUtils::stringToVec3(std::string line)
{
    glm::vec3 output(0.0f);
    size_t index;
    std::string toParse;

    for (size_t i = 0; i < 3; i++)
    {
        index = line.find_first_of(' ');
        toParse = line.substr(0, index);
        output[i] = std::stof(toParse);
        line = line.substr(index + 1);
    }

    return output;
}

glm::quat TerrainUtils::stringToQuat(std::string line)
{
    glm::quat output;
    size_t index;
    std::string toParse;

    float* quatElems[] = {&output.w, &output.x, &output.y, &output.z};
    for (size_t i = 0; i < 4; i++)
    {
        index = line.find_first_of(' ');
        toParse = line.substr(0, index);
        *quatElems[i] = std::stof(toParse);
        line = line.substr(index + 1);
    }

    return output;
}