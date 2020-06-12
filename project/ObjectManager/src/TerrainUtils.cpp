#include "TerrainUtils.hpp"

#include "Core.hpp"

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
    glm::vec2 uv0s = {4.0f, 4.0f};
    terrainMat->setVec2("uv0scale", uv0s);
    terrainMat->setTexture("diffuse0", grass1Diffuse);
    terrainMat->setTexture("normal0", grass1Normal);
    terrainMat->setTexture("orm0", grass1Specular);
    glm::vec2 uv1s = {10.0f, 10.0f};
    terrainMat->setVec2("uv1scale", uv1s);
    terrainMat->setTexture("diffuse1", gravel1Diffuse);
    terrainMat->setTexture("normal1", gravel1Normal);
    terrainMat->setTexture("orm1", gravel1Specular);
    glm::vec2 uv2s = {4.0f, 4.0f};
    terrainMat->setVec2("uv2scale", uv2s);
    terrainMat->setTexture("diffuse2", rockGrassDiffuse);
    terrainMat->setTexture("normal2", rockGrassNormal);
    terrainMat->setTexture("orm2", occroumetTex);
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