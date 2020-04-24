#include "SceneWriter.hpp"


#include "Shader.hpp"
#include "Material.hpp"
#include "Texture.hpp"
#include "BillboardRenderer.inl"

#include <json.hpp>

void SceneWriter::tempScene()
{
    ar.loadAudioClip("Resources/Audios/sample.wav");
    ar.loadShader("Resources/Shaders/UnlitColor/UnlitColor.vert");
    ar.loadShader("Resources/Shaders/UnlitColor/UnlitColor.frag");
    ar.loadShader("Resources/Shaders/UnlitTexture/UnlitTexture.vert");
    ar.loadShader("Resources/Shaders/UnlitTexture/UnlitTexture.frag");
    ar.loadShader("Resources/Shaders/UnlitBillboardInstanced/UnlitBillboardInstanced.vert");
    ar.loadShader("Resources/Shaders/UnlitBillboardInstanced/UnlitBillboardInstanced.frag");
    ar.loadShader("Resources/Shaders/UnlitSkinned/UnlitSkinned.vert");
    ar.loadShader("Resources/Shaders/UnlitSkinned/UnlitSkinned.frag");
    ar.loadShader("Resources/Shaders/SkyboxCubemap/SkyboxCubemap.vert");
    ar.loadShader("Resources/Shaders/SkyboxCubemap/SkyboxCubemap.frag");
    ar.loadMesh("Resources/Models/Test.fbx");
    ar.loadMesh("Resources/Models/unit_sphere.fbx");
    ar.loadSkinnedMesh("Resources/Models/House Dancing.fbx");
    ar.loadTexture("Resources/Textures/tex.png");
    ar.loadTexture("Resources/Textures/skybox/nx.png");
    ar.loadTexture("Resources/Textures/skybox/ny.png");
    ar.loadTexture("Resources/Textures/skybox/nz.png");
    ar.loadTexture("Resources/Textures/skybox/px.png");
    ar.loadTexture("Resources/Textures/skybox/py.png");
    ar.loadTexture("Resources/Textures/skybox/pz.png");

    auto unlitColor = Shader(ar.shaders.find("Resources/Shaders/UnlitColor/UnlitColor.vert")->second.c_str(),
                        ar.shaders.find("Resources/Shaders/UnlitColor/UnlitColor.frag")->second.c_str());

    auto unlitTexture = Shader(  ar.shaders.find("Resources/Shaders/UnlitTexture/UnlitTexture.vert")->second.c_str(),
                            ar.shaders.find("Resources/Shaders/UnlitTexture/UnlitTexture.frag")->second.c_str());

    auto unlitInstanced = Shader(ar.shaders.find("Resources/Shaders/UnlitBillboardInstanced/UnlitBillboardInstanced.vert")->second.c_str(),
                            ar.shaders.find("Resources/Shaders/UnlitBillboardInstanced/UnlitBillboardInstanced.frag")->second.c_str());

    auto skyboxShader = Shader(  ar.shaders.find("Resources/Shaders/SkyboxCubemap/SkyboxCubemap.vert")->second.c_str(),
                            ar.shaders.find("Resources/Shaders/SkyboxCubemap/SkyboxCubemap.frag")->second.c_str());

    auto skinnedShader = Shader( ar.shaders.find("Resources/Shaders/UnlitSkinned/UnlitSkinned.vert")->second.c_str(),
                            ar.shaders.find("Resources/Shaders/UnlitSkinned/UnlitSkinned.frag")->second.c_str());

    auto unlitColorMat = Material(&unlitColor);
    unlitColorMat.setVec4("color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

    TextureData texData = ar.textures.find("Resources/Textures/tex.png")->second;

    TextureCreateInfo texCreateInfo = {};
    texCreateInfo.format = GL_RGBA;
    texCreateInfo.generateMipmaps = true;
    texCreateInfo.magFilter = GL_LINEAR;
    texCreateInfo.minFilter = GL_LINEAR_MIPMAP_LINEAR;
    texCreateInfo.wrapMode = GL_CLAMP_TO_EDGE;
    texCreateInfo.width = texData.width;
    texCreateInfo.height = texData.height;
    Texture texture(texData.data, texCreateInfo, "Resources/Textures/tex.png");

    auto unlitTextureMat = Material(&unlitTexture);
    unlitTextureMat.setTexture("mainTex", texture);

    auto unlitInstancedMat = Material(&unlitInstanced);
    unlitInstancedMat.setTexture("mainTex", texture);

    TextureData skyboxNX = ar.textures.find("Resources/Textures/skybox/nx.png")->second;
    TextureData skyboxNY = ar.textures.find("Resources/Textures/skybox/ny.png")->second;
    TextureData skyboxNZ = ar.textures.find("Resources/Textures/skybox/nz.png")->second;
    TextureData skyboxPX = ar.textures.find("Resources/Textures/skybox/px.png")->second;
    TextureData skyboxPY = ar.textures.find("Resources/Textures/skybox/py.png")->second;
    TextureData skyboxPZ = ar.textures.find("Resources/Textures/skybox/pz.png")->second;
    TextureCreateInfo skyboxCreateInfo = {};
    skyboxCreateInfo.format = GL_RGBA;
    skyboxCreateInfo.generateMipmaps = true;
    skyboxCreateInfo.magFilter = GL_LINEAR;
    skyboxCreateInfo.minFilter = GL_LINEAR;
    skyboxCreateInfo.wrapMode = GL_CLAMP_TO_EDGE;
    skyboxCreateInfo.width = skyboxNX.width;
    skyboxCreateInfo.height = skyboxNX.height;
    Cubemap cubemap(
        skyboxCreateInfo,
        skyboxNZ.data,
        skyboxNX.data,
        skyboxPX.data,
        skyboxPZ.data,
        skyboxPY.data,
        skyboxNY.data
    );

    auto skyboxMat = Material(&skyboxShader);
    skyboxMat.setCubemap("cubemap", cubemap);

    auto skinnedMat = Material(&skinnedShader);

    om.NewEntity(2);
    {
        auto mr = om.NewComponent<SkinnedMeshRenderer>();
            mr->material = &skinnedMat;
            mr->mesh = &ar.skinnedMeshes.find("Resources/Models/House Dancing.fbx/Alpha_Surface")->second;

        auto t = om.NewComponent<Transform>();
            t->getLocalPositionModifiable() = { 0.0f, 0.0f, -25.0f };
            t->setParent(&sm.rootNode);
    }
    
    om.NewEntity(2);
    {
        auto mr = om.NewComponent<SkinnedMeshRenderer>();
            mr->material = &skinnedMat;
            mr->mesh = &ar.skinnedMeshes.find("Resources/Models/House Dancing.fbx/Alpha_Joints")->second;

        auto t = om.NewComponent<Transform>();
            t->getLocalPositionModifiable() = { 0.0f, 0.0f, -25.0f };
            t->setParent(&sm.rootNode);
    }

    om.NewEntity(2);
    {
        auto br = om.NewComponent<BillboardRenderer>();
            br->material = &unlitInstancedMat;
        
        auto t = om.NewComponent<Transform>();
            t->getLocalPositionModifiable() = { 10.0f, 0.0f, 0.0f };
            t->getLocalScaleModifiable() = { 10.0f, 10.0f, 10.0f };
    }

    om.NewEntity(2);
    {
        auto br = om.NewComponent<BillboardRenderer>();
            br->material = &unlitInstancedMat;
        
        auto t = om.NewComponent<Transform>();
            t->getLocalPositionModifiable() = { 0.0f, 0.0f, 0.0f };
            t->getLocalScaleModifiable() = { 10.0f, 10.0f, 10.0f };
    }

    om.NewEntity(2);
    {
        auto br = om.NewComponent<BillboardRenderer>();
            br->material = &unlitInstancedMat;
        
        auto t = om.NewComponent<Transform>();
            t->getLocalPositionModifiable() = { -10.0f, 0.0f, 0.0f };
            t->getLocalScaleModifiable() = { 10.0f, 10.0f, 10.0f };
    }

    om.NewEntity(2);
    {
        auto br = om.NewComponent<BillboardRenderer>();
            br->material = &unlitInstancedMat;
        
        auto t = om.NewComponent<Transform>();
            t->getLocalPositionModifiable() = { 10.0f, 10.0f, 0.0f };
            t->getLocalScaleModifiable() = { 10.0f, 10.0f, 10.0f };
    }

    om.NewEntity(2);
    {
        auto br = om.NewComponent<BillboardRenderer>();
            br->material = &unlitInstancedMat;
        
        auto t = om.NewComponent<Transform>();
            t->getLocalPositionModifiable() = { 0.0f, 10.0f, 0.0f };
            t->getLocalScaleModifiable() = { 10.0f, 10.0f, 10.0f };
    }

    om.NewEntity(2);
    {
        auto br = om.NewComponent<BillboardRenderer>();
            br->material = &unlitInstancedMat;
        
        auto t = om.NewComponent<Transform>();
            t->getLocalPositionModifiable() = { -10.0f, 10.0f, 0.0f };
            t->getLocalScaleModifiable() = { 10.0f, 10.0f, 10.0f };
    }

    om.NewEntity(4);
    {
        auto t = om.NewComponent<Transform>();
            t->getLocalPositionModifiable() = { 0.5f, 0.0f, 10.0f };
            t->getLocalScaleModifiable() *= 10;
            t->setParent(&sm.rootNode);

        auto c = om.NewComponent<SphereCollider>();
            c->radius = 10;

        auto mr = om.NewComponent<MeshRenderer>();
            mr->mesh = &ar.meshes.find("Resources/Models/unit_sphere.fbx/Sphere001")->second;
            mr->material = &unlitTextureMat;

        auto so1 = om.NewComponent<AudioSource>();
            so1->getClipsModifiable().push_back("Resources/Audios/test.wav");
            so1->getIsRelativeModifiable() = false;
            so1->getIsLoopingModifiable() = true;
    }

    om.NewEntity(3);
    {
        auto t = om.NewComponent<Transform>();
            t->getLocalPositionModifiable()={-0.5f,0.0f,10.0f};
            t->getLocalScaleModifiable() *= 10;
            t->setParent(&sm.rootNode);

        auto c = om.NewComponent<SphereCollider>();
            c->radius = 10;

        auto mr = om.NewComponent<MeshRenderer>();
            mr->mesh = &ar.meshes.find("Resources/Models/unit_sphere.fbx/Sphere001")->second;
            mr->material = &unlitColorMat;
    }

}

void SceneWriter::saveScene()
{
    nlohmann::json j;
    for( Entity e : om.entities)
    {

    }
}