//#include "Core.hpp"
#include "SceneWriter.hpp"
#include "Shader.hpp"
#include "Material.hpp"
#include "Texture.hpp"
#include "BillboardRenderer.inl"

int main()
{
    // Core core;

    // if( int errorCode = core.init() )
    // {
    //     return errorCode;
    // }

    // int exitCode = core.mainLoop();

    // core.cleanup();

    // return exitCode;

    SceneWriter sw;

    sw.ar.objectModulePtr = &sw.om;
    sw.ar.sceneModulePtr = &sw.sm;

    sw.ar.loadAudioClip("Resources/Audios/sample.wav");
    sw.ar.loadShader("Resources/Shaders/UnlitColor/UnlitColor.vert");
    sw.ar.loadShader("Resources/Shaders/UnlitColor/UnlitColor.frag");
    sw.ar.loadShader("Resources/Shaders/UnlitTexture/UnlitTexture.vert");
    sw.ar.loadShader("Resources/Shaders/UnlitTexture/UnlitTexture.frag");
    sw.ar.loadShader("Resources/Shaders/UnlitBillboardInstanced/UnlitBillboardInstanced.vert");
    sw.ar.loadShader("Resources/Shaders/UnlitBillboardInstanced/UnlitBillboardInstanced.frag");
    sw.ar.loadShader("Resources/Shaders/UnlitSkinned/UnlitSkinned.vert");
    sw.ar.loadShader("Resources/Shaders/UnlitSkinned/UnlitSkinned.frag");
    sw.ar.loadShader("Resources/Shaders/SkyboxCubemap/SkyboxCubemap.vert");
    sw.ar.loadShader("Resources/Shaders/SkyboxCubemap/SkyboxCubemap.frag");
    sw.ar.loadMesh("Resources/Models/Test.fbx");
    sw.ar.loadMesh("Resources/Models/unit_sphere.fbx");
    sw.ar.loadSkinnedMesh("Resources/Models/House Dancing.fbx");
    sw.ar.loadTexture("Resources/Textures/tex.png");
    sw.ar.loadTexture("Resources/Textures/skybox/nx.png");
    sw.ar.loadTexture("Resources/Textures/skybox/ny.png");
    sw.ar.loadTexture("Resources/Textures/skybox/nz.png");
    sw.ar.loadTexture("Resources/Textures/skybox/px.png");
    sw.ar.loadTexture("Resources/Textures/skybox/py.png");
    sw.ar.loadTexture("Resources/Textures/skybox/pz.png");

    auto unlitColor = Shader(sw.ar.shaders.find("Resources/Shaders/UnlitColor/UnlitColor.vert")->second.c_str(),
                        sw.ar.shaders.find("Resources/Shaders/UnlitColor/UnlitColor.frag")->second.c_str());

    auto unlitTexture = Shader(  sw.ar.shaders.find("Resources/Shaders/UnlitTexture/UnlitTexture.vert")->second.c_str(),
                            sw.ar.shaders.find("Resources/Shaders/UnlitTexture/UnlitTexture.frag")->second.c_str());

    auto unlitInstanced = Shader(sw.ar.shaders.find("Resources/Shaders/UnlitBillboardInstanced/UnlitBillboardInstanced.vert")->second.c_str(),
                            sw.ar.shaders.find("Resources/Shaders/UnlitBillboardInstanced/UnlitBillboardInstanced.frag")->second.c_str());

    auto skyboxShader = Shader(  sw.ar.shaders.find("Resources/Shaders/SkyboxCubemap/SkyboxCubemap.vert")->second.c_str(),
                            sw.ar.shaders.find("Resources/Shaders/SkyboxCubemap/SkyboxCubemap.frag")->second.c_str());

    auto skinnedShader = Shader( sw.ar.shaders.find("Resources/Shaders/UnlitSkinned/UnlitSkinned.vert")->second.c_str(),
                            sw.ar.shaders.find("Resources/Shaders/UnlitSkinned/UnlitSkinned.frag")->second.c_str());

    std::cout << "Shaders compiled" <<std::endl;
    auto unlitColorMat = Material(&unlitColor);
    //unlitColorMat.setVec4("color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

    TextureData texData = sw.ar.textures.find("Resources/Textures/tex.png")->second;

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

    TextureData skyboxNX = sw.ar.textures.find("Resources/Textures/skybox/nx.png")->second;
    TextureData skyboxNY = sw.ar.textures.find("Resources/Textures/skybox/ny.png")->second;
    TextureData skyboxNZ = sw.ar.textures.find("Resources/Textures/skybox/nz.png")->second;
    TextureData skyboxPX = sw.ar.textures.find("Resources/Textures/skybox/px.png")->second;
    TextureData skyboxPY = sw.ar.textures.find("Resources/Textures/skybox/py.png")->second;
    TextureData skyboxPZ = sw.ar.textures.find("Resources/Textures/skybox/pz.png")->second;
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

    sw.om.NewEntity(2);
    {
        auto mr = sw.om.NewComponent<SkinnedMeshRenderer>();
            mr->material = &skinnedMat;
            mr->mesh = &sw.ar.skinnedMeshes.find("Resources/Models/House Dancing.fbx/Alpha_Surface")->second;

        auto t = sw.om.NewComponent<Transform>();
            t->getLocalPositionModifiable() = { 0.0f, 0.0f, -25.0f };
            t->setParent(&sw.sm.rootNode);
    }
    
    sw.om.NewEntity(2);
    {
        auto mr = sw.om.NewComponent<SkinnedMeshRenderer>();
            mr->material = &skinnedMat;
            mr->mesh = &sw.ar.skinnedMeshes.find("Resources/Models/House Dancing.fbx/Alpha_Joints")->second;

        auto t = sw.om.NewComponent<Transform>();
            t->getLocalPositionModifiable() = { 0.0f, 0.0f, -25.0f };
            t->setParent(&sw.sm.rootNode);
    }

    sw.om.NewEntity(2);
    {
        auto br = sw.om.NewComponent<BillboardRenderer>();
            br->material = &unlitInstancedMat;
        
        auto t = sw.om.NewComponent<Transform>();
            t->getLocalPositionModifiable() = { 10.0f, 0.0f, 0.0f };
            t->getLocalScaleModifiable() = { 10.0f, 10.0f, 10.0f };
            t->setParent(&sw.sm.rootNode);
    }

    sw.om.NewEntity(2);
    {
        auto br = sw.om.NewComponent<BillboardRenderer>();
            br->material = &unlitInstancedMat;
        
        auto t = sw.om.NewComponent<Transform>();
            t->getLocalPositionModifiable() = { 0.0f, 0.0f, 0.0f };
            t->getLocalScaleModifiable() = { 10.0f, 10.0f, 10.0f };
            t->setParent(&sw.sm.rootNode);
    }

    sw.om.NewEntity(2);
    {
        auto br = sw.om.NewComponent<BillboardRenderer>();
            br->material = &unlitInstancedMat;
        
        auto t = sw.om.NewComponent<Transform>();
            t->getLocalPositionModifiable() = { -10.0f, 0.0f, 0.0f };
            t->getLocalScaleModifiable() = { 10.0f, 10.0f, 10.0f };
            t->setParent(&sw.sm.rootNode);

    }

    sw.om.NewEntity(2);
    {
        auto br = sw.om.NewComponent<BillboardRenderer>();
            br->material = &unlitInstancedMat;
        
        auto t = sw.om.NewComponent<Transform>();
            t->getLocalPositionModifiable() = { 10.0f, 10.0f, 0.0f };
            t->getLocalScaleModifiable() = { 10.0f, 10.0f, 10.0f };
            t->setParent(&sw.sm.rootNode);
    }

    sw.om.NewEntity(2);
    {
        auto br = sw.om.NewComponent<BillboardRenderer>();
            br->material = &unlitInstancedMat;
        
        auto t = sw.om.NewComponent<Transform>();
            t->getLocalPositionModifiable() = { 0.0f, 10.0f, 0.0f };
            t->getLocalScaleModifiable() = { 10.0f, 10.0f, 10.0f };
            t->setParent(&sw.sm.rootNode);
    }

    sw.om.NewEntity(2);
    {
        auto br = sw.om.NewComponent<BillboardRenderer>();
            br->material = &unlitInstancedMat;
        
        auto t = sw.om.NewComponent<Transform>();
            t->getLocalPositionModifiable() = { -10.0f, 10.0f, 0.0f };
            t->getLocalScaleModifiable() = { 10.0f, 10.0f, 10.0f };
            t->setParent(&sw.sm.rootNode);
    }

    sw.om.NewEntity(4);
    {
        auto t = sw.om.NewComponent<Transform>();
            t->getLocalPositionModifiable() = { 0.5f, 0.0f, 10.0f };
            t->getLocalScaleModifiable() *= 10;
            t->setParent(&sw.sm.rootNode);

        auto c = sw.om.NewComponent<SphereCollider>();
            c->radius = 10;

        auto mr = sw.om.NewComponent<MeshRenderer>();
            mr->mesh = &sw.ar.meshes.find("Resources/Models/unit_sphere.fbx/Sphere001")->second;
            mr->material = &unlitTextureMat;

        auto so1 = sw.om.NewComponent<AudioSource>();
            so1->getClipsModifiable().push_back("Resources/Audios/test.wav");
            so1->getIsRelativeModifiable() = false;
            so1->getIsLoopingModifiable() = true;
    }

    sw.om.NewEntity(3);
    {
        auto t = sw.om.NewComponent<Transform>();
            t->getLocalPositionModifiable()={-0.5f,0.0f,10.0f};
            t->getLocalScaleModifiable() *= 10;
            t->setParent(&sw.sm.rootNode);

        auto c = sw.om.NewComponent<SphereCollider>();
            c->radius = 10;

        auto mr = sw.om.NewComponent<MeshRenderer>();
            mr->mesh = &sw.ar.meshes.find("Resources/Models/unit_sphere.fbx/Sphere001")->second;
            mr->material = &unlitColorMat;
    }

    sw.saveScene();
}