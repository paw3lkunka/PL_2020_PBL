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

    auto sand1Diffuse = GetCore().objectModule.newTexture("Resources/Textures/Unity/Sand_albedo.png", texCreateInfo);
    auto sand1Normal = GetCore().objectModule.newTexture("Resources/Textures/Unity/Sand_normal.png", texCreateInfo);
    auto sand1Specular = GetCore().objectModule.newTexture("Resources/Textures/Unity/Sand_occroumet.png", texCreateInfo);

    auto whiteTex = GetCore().objectModule.newTexture("Resources/Textures/white.png", texCreateInfo);
    auto blackTex = GetCore().objectModule.newTexture("Resources/Textures/black.png", texCreateInfo);
    auto bumpTex = GetCore().objectModule.newTexture("Resources/Textures/bump.png", texCreateInfo);
    auto occroumetTex = GetCore().objectModule.newTexture("Resources/Textures/occroumet.png", texCreateInfo);

    Material* terrainMat = GetCore().objectModule.newMaterial(terrainShader, "terrainMat", RenderType::Opaque);
    glm::vec2 uv0s = {4.0f, 4.0f};
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
    glm::vec2 uv3s = {8.0f, 8.0f};
    terrainMat->setVec2("uv3scale", uv3s);
    terrainMat->setTexture("diffuse3", sand1Diffuse);
    terrainMat->setTexture("normal3", sand1Normal);
    terrainMat->setTexture("orm3", sand1Specular);

    Entity* terrain = GetCore().objectModule.newEntity(1, "Terrain");
    auto terrainRoot = GetCore().objectModule.newEmptyComponentForLastEntity<Transform>();
    terrainRoot->getLocalPositionModifiable() = glm::vec3(50, -99.5, -50);
    terrainRoot->setParent(&GetCore().sceneModule.rootNode);

    namespace fs = std::filesystem;
    std::string path = "Resources/Terrain/";
    for (auto& entry : fs::directory_iterator(path))
    {
        GetCore().objectModule.newModel(entry.path().string().c_str());
        std::string filename = entry.path().filename().string();
        Entity* entity = GetCore().objectModule.getEntityPtrByName((filename + "/defaultobject").c_str());
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
        Entity* entity = GetCore().objectModule.getEntityPtrByName((filename +  ".obj/defaultobject").c_str());
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
        else if (entry.path().filename().string() == "colliders.txt")
        {
            // do nothing
        }
        else
        {
            GetCore().objectModule.newModel(entry.path().string().c_str(), true);
        }
    }

    Entity* propsRoot = GetCore().objectModule.newEntity(1, "propsRoot");
    auto propsRootTransform = GetCore().objectModule.newEmptyComponentForLastEntity<Transform>();
        propsRootTransform->setParent(&GetCore().sceneModule.rootNode);
        propsRootTransform->getLocalPositionModifiable() = glm::vec3(50, -99.5, -50);

    std::fstream exportInfo;
    exportInfo.open(exportInfoPath.path().string().c_str(), std::ios::in);
    if (exportInfo.is_open())
    {
        std::map<std::string, int> loadedModels;
        std::vector<MeshRenderer*> currentSubmeshes;
        int submeshIndex = 0;
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
                currentSubmeshes.clear();
                submeshIndex = 0;

                modelPath = line.substr(line.find_first_of(':') + 1);
                std::getline(exportInfo, line);
                position = stringToVec3(line);
                position.x = -position.x;
                std::getline(exportInfo, line);
                rotation = stringToQuat(line);
                std::getline(exportInfo, line);
                scale = stringToVec3(line);

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
                            currentSubmeshes.push_back(mr);
                        }
                    }
                    else
                    {
                        auto mr = GetCore().objectModule.newEmptyComponentForLastEntity<MeshRenderer>();
                            mr->mesh = existingModel->getComponentPtr<MeshRenderer>()->mesh;
                            mr->material = existingModel->getComponentPtr<MeshRenderer>()->material;
                        currentSubmeshes.push_back(mr);
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

                    int entitiesToCreate = t->children.size();
                    if (entitiesToCreate > 1)
                    {
                        auto children = existingModel->getComponentPtr<Transform>()->children;
                        for(auto child : children)
                        {
                            auto mr = child->entityPtr->getComponentPtr<MeshRenderer>();
                            currentSubmeshes.push_back(mr);
                        }
                    }
                    else
                    {
                        currentSubmeshes.push_back(existingModel->getComponentPtr<MeshRenderer>());
                    }
                }
            }

            index = line.find_first_of('%');
            if (index != std::string::npos)
            {
                std::string materialName = line.substr(line.find_first_of(':') + 1);
                auto materialPtr = GetCore().objectModule.getMaterialPtrByName(materialName.c_str());
                if (materialPtr != nullptr)
                {
                    currentSubmeshes[submeshIndex]->material = materialPtr;
                }
                submeshIndex++;
            }
        }
    }
    else
    {
        std::cerr << "exportinfo.txt file not found!\n";
    }
}

void TerrainUtils::createMaterialsForModels()
{
    Shader* pbr = GetCore().objectModule.getShaderPtrByName("standardPbr");
    Shader* pbrCutout = GetCore().objectModule.newShader("standardPbrCutout", "Resources/Shaders/StandardPBRCutout/StandardPBRCutout.vert", "Resources/Shaders/StandardPBRCutout/StandardPBRCutout.frag");

    TextureCreateInfo tci = {};
    tci.generateMipmaps = true;
    tci.magFilter = GL_LINEAR;
    tci.minFilter = GL_LINEAR_MIPMAP_LINEAR;
    tci.wrapMode = GL_REPEAT;

    TextureCreateInfo tci_clamp = {};
    tci_clamp.generateMipmaps = true;
    tci_clamp.magFilter = GL_LINEAR;
    tci_clamp.minFilter = GL_LINEAR_MIPMAP_LINEAR;
    tci_clamp.wrapMode = GL_CLAMP_TO_EDGE;

    auto emptyOccRouMet = GetCore().objectModule.newTexture("Resources/Textures/occroumet.png", tci);

    auto albedo = GetCore().objectModule.newTexture("Resources/Textures/Unity/Conifer Bark Array BODT-albedo.png", tci);
    auto normal = GetCore().objectModule.newTexture("Resources/Textures/Unity/Conifer Bark Array BODT-normal.png", tci);
    auto occRouMet = GetCore().objectModule.newTexture("Resources/Textures/Unity/Conifer Bark Array BODT-occRouMet.png", tci);

    auto material = GetCore().objectModule.newMaterial(pbr, "Conifer Bark Array BODT", RenderType::Opaque, false);
    material->setTexture("diffuse", albedo);
    material->setTexture("normal", normal);
    material->setTexture("occRouMet", occRouMet);

    material = GetCore().objectModule.newMaterial(pbr, "Conifer Bark Simple BODT", RenderType::Opaque, false);
    material->setTexture("diffuse", albedo);
    material->setTexture("normal", normal);
    material->setTexture("occRouMet", occRouMet);

    albedo = GetCore().objectModule.newTexture("Resources/Textures/Unity/BODT Conifer Branches-albedo.png", tci_clamp);
    normal = GetCore().objectModule.newTexture("Resources/Textures/Unity/BODT Conifer Branches-normal.png", tci_clamp);

    material = GetCore().objectModule.newMaterial(pbrCutout, "Conifer Leaves BODT", RenderType::Opaque, false);
    material->setTexture("diffuse", albedo);
    material->setTexture("normal", normal);
    material->setTexture("occRouMet", emptyOccRouMet);
    material->setFloat("cutoff", 0.3f);

    albedo = GetCore().objectModule.newTexture("Resources/Textures/Unity/T_Rock_01_D.TGA", tci);
    normal = GetCore().objectModule.newTexture("Resources/Textures/Unity/T_Rock_01_N.TGA", tci);

    material = GetCore().objectModule.newMaterial(pbr, "M_Rock_01", RenderType::Opaque, false);
    material->setTexture("diffuse", albedo);
    material->setTexture("normal", normal);
    material->setTexture("occRouMet", emptyOccRouMet);

    albedo = GetCore().objectModule.newTexture("Resources/Textures/Unity/T_Rock_02_D.TGA", tci);
    normal = GetCore().objectModule.newTexture("Resources/Textures/Unity/T_Rock_02_N.TGA", tci);

    material = GetCore().objectModule.newMaterial(pbr, "M_Rock_02", RenderType::Opaque, false);
    material->setTexture("diffuse", albedo);
    material->setTexture("normal", normal);
    material->setTexture("occRouMet", emptyOccRouMet);

    albedo = GetCore().objectModule.newTexture("Resources/Textures/Unity/T_Rock_03_D.TGA", tci);
    normal = GetCore().objectModule.newTexture("Resources/Textures/Unity/T_Rock_03_N.TGA", tci);

    material = GetCore().objectModule.newMaterial(pbr, "M_Rock_03", RenderType::Opaque, false);
    material->setTexture("diffuse", albedo);
    material->setTexture("normal", normal);
    material->setTexture("occRouMet", emptyOccRouMet);

    albedo = GetCore().objectModule.newTexture("Resources/Textures/Unity/T_Rock_04_D.TGA", tci);
    normal = GetCore().objectModule.newTexture("Resources/Textures/Unity/T_Rock_04_N.TGA", tci);

    material = GetCore().objectModule.newMaterial(pbr, "M_Rock_04", RenderType::Opaque, false);
    material->setTexture("diffuse", albedo);
    material->setTexture("normal", normal);
    material->setTexture("occRouMet", emptyOccRouMet);

    albedo = GetCore().objectModule.newTexture("Resources/Textures/Unity/T_Rock_05_D.TGA", tci);
    normal = GetCore().objectModule.newTexture("Resources/Textures/Unity/T_Rock_05_N.TGA", tci);

    material = GetCore().objectModule.newMaterial(pbr, "M_Rock_05", RenderType::Opaque, false);
    material->setTexture("diffuse", albedo);
    material->setTexture("normal", normal);
    material->setTexture("occRouMet", emptyOccRouMet);

    albedo = GetCore().objectModule.newTexture("Resources/Textures/Unity/T_Rock_06_D.TGA", tci);
    normal = GetCore().objectModule.newTexture("Resources/Textures/Unity/T_Rock_06_N.TGA", tci);

    material = GetCore().objectModule.newMaterial(pbr, "M_Rock_06", RenderType::Opaque, false);
    material->setTexture("diffuse", albedo);
    material->setTexture("normal", normal);
    material->setTexture("occRouMet", emptyOccRouMet);

    albedo = GetCore().objectModule.newTexture("Resources/Textures/Unity/T_Rock_06_D.TGA", tci);
    normal = GetCore().objectModule.newTexture("Resources/Textures/Unity/T_Rock_06_N.TGA", tci);

    material = GetCore().objectModule.newMaterial(pbr, "M_Rock_06", RenderType::Opaque, false);
    material->setTexture("diffuse", albedo);
    material->setTexture("normal", normal);
    material->setTexture("occRouMet", emptyOccRouMet);

    albedo = GetCore().objectModule.newTexture("Resources/Textures/Unity/T_Rock_06_D.TGA", tci);
    normal = GetCore().objectModule.newTexture("Resources/Textures/Unity/T_Rock_06_N.TGA", tci);

    material = GetCore().objectModule.newMaterial(pbr, "M_Rock_06", RenderType::Opaque, false);
    material->setTexture("diffuse", albedo);
    material->setTexture("normal", normal);
    material->setTexture("occRouMet", emptyOccRouMet);

    albedo = GetCore().objectModule.newTexture("Resources/Textures/Unity/rushes-diffuse.tga", tci_clamp);
    normal = GetCore().objectModule.newTexture("Resources/Textures/Unity/rushes-normal.tga", tci_clamp);

    material = GetCore().objectModule.newMaterial(pbrCutout, "Grass_rushes", RenderType::Opaque, false);
    material->setTexture("diffuse", albedo);
    material->setTexture("normal", normal);
    material->setTexture("occRouMet", emptyOccRouMet);
    material->setFloat("cutoff", 0.7f);
}

void TerrainUtils::importColliders()
{
    Entity* colliders = GetCore().objectModule.newEntity(1, "collidersRoot");
    auto collidersRoot = GetCore().objectModule.newEmptyComponentForLastEntity<Transform>();
        collidersRoot->getLocalPositionModifiable() = glm::vec3(50, -99.5, -50);
        collidersRoot->setParent(&GetCore().sceneModule.rootNode);


    std::fstream colliderInfo;
    colliderInfo.open("Resources/Models/Unity/colliders.txt", std::ios::in);
    if (colliderInfo.is_open())
    {
        //GetCore().objectModule.newModel("Resources/Models/Box.FBX", false, false);
        std::string line;
        glm::vec3 position;
        glm::quat rotation;
        glm::vec3 halfSize;
        size_t index = 0;
        int i = 0;

        while(std::getline(colliderInfo, line))
        {
            index = line.find_first_of('$');
            if (index != std::string::npos)
            {
                std::getline(colliderInfo, line);
                position = stringToVec3(line);
                position.x = -position.x;
                std::getline(colliderInfo, line);
                rotation = stringToQuat(line);
                std::getline(colliderInfo, line);
                halfSize = stringToVec3(line);

                Entity* colliderEntity = GetCore().objectModule.newEntity(3, "ImpCollider" + std::to_string(i++));

                auto t = GetCore().objectModule.newEmptyComponentForLastEntity<Transform>();
                    t->getLocalPositionModifiable() = position;
                    t->getLocalRotationModifiable() = rotation;
                    t->getLocalScaleModifiable() = halfSize * 2.0f;
                    t->setParent(collidersRoot);
                // auto mr = GetCore().objectModule.newEmptyComponentForLastEntity<MeshRenderer>();
                //     mr->mesh = GetCore().objectModule.getMeshCustomPtrByPath("Resources/Models/Box.FBX/Box001");
                auto col = GetCore().objectModule.newEmptyComponentForLastEntity<BoxCollider>();
                    col->halfSize = halfSize;
                auto rb = GetCore().objectModule.newEmptyComponentForLastEntity<Rigidbody>();
                    rb->type = rp3d::BodyType::STATIC;
            }
        }
    }
    else
    {
        std::cout << "Unable to read colliders!!!!\n";
    }

    std::fstream hideSpotInfo;
    hideSpotInfo.open("Resources/hideColliders.txt", std::ios::in);
    if (hideSpotInfo.is_open())
    {
        //GetCore().objectModule.newModel("Resources/Models/Box.FBX", false, false);
        std::string line;
        glm::vec3 position;
        glm::quat rotation;
        glm::vec3 halfSize;
        size_t index = 0;
        int i = 0;

        while(std::getline(hideSpotInfo, line))
        {
            index = line.find_first_of('$');
            if (index != std::string::npos)
            {
                std::getline(hideSpotInfo, line);
                position = stringToVec3(line);
                position.x = -position.x;
                std::getline(hideSpotInfo, line);
                rotation = stringToQuat(line);
                std::getline(hideSpotInfo, line);
                halfSize = stringToVec3(line);

                Entity* colliderEntity = GetCore().objectModule.newEntity(4, "HideZone" + std::to_string(i++));

                auto t = GetCore().objectModule.newEmptyComponentForLastEntity<Transform>();
                    t->getLocalPositionModifiable() = position;
                    t->getLocalRotationModifiable() = rotation;
                    t->getLocalScaleModifiable() = halfSize * 2.0f;
                    t->setParent(collidersRoot);
                // auto mr = GetCore().objectModule.newEmptyComponentForLastEntity<MeshRenderer>();
                //     mr->mesh = GetCore().objectModule.getMeshCustomPtrByPath("Resources/Models/Box.FBX/Box001");
                auto col = GetCore().objectModule.newEmptyComponentForLastEntity<BoxCollider>();
                    col->halfSize = halfSize;
                    col->isTrigger = true;
                auto rb = GetCore().objectModule.newEmptyComponentForLastEntity<Rigidbody>();
                    rb->type = rp3d::BodyType::STATIC;
                auto hs = GetCore().objectModule.newEmptyComponentForLastEntity<Hideout>();
            }
        }
    }
    else
    {
        std::cout << "Unable to read colliders!!!!\n";
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