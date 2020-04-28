//#include "Core.hpp"
#include "ObjectModule.hpp"
#include "SceneModule.hpp"
#include "Shader.hpp"
#include "Material.hpp"
#include "Texture.hpp"
#include "BillboardRenderer.inl"
#include "Components.inc"

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

    ObjectModule objMod;
    SceneModule sceneModule;

    objMod.assetReader.loadAudioClip("Resources/Audios/sample.wav");
    objMod.assetReader.loadMesh("Resources/Models/Test.fbx");
    objMod.assetReader.loadMesh("Resources/Models/unit_sphere.fbx");
    //objMod.assetReader.loadSkinnedMesh("Resources/Models/House Dancing.fbx");

    auto unlitColor = objMod.objectMaker.newShader("Resources/Shaders/UnlitColor/UnlitColor.vert", "Resources/Shaders/UnlitColor/UnlitColor.frag");
    

    auto unlitTexture = objMod.objectMaker.newShader("Resources/Shaders/UnlitTexture/UnlitTexture.vert", "Resources/Shaders/UnlitTexture/UnlitTexture.frag");

    auto unlitInstanced = objMod.objectMaker.newShader("Resources/Shaders/UnlitBillboardInstanced/UnlitBillboardInstanced.vert", "Resources/Shaders/UnlitBillboardInstanced/UnlitBillboardInstanced.frag");
    

    auto skyboxShader = objMod.objectMaker.newShader("Resources/Shaders/SkyboxCubemap/SkyboxCubemap.vert", "Resources/Shaders/SkyboxCubemap/SkyboxCubemap.frag");

    auto skinnedShader = objMod.objectMaker.newShader("Resources/Shaders/UnlitSkinned/UnlitSkinned.vert", "Resources/Shaders/UnlitSkinned/UnlitSkinned.frag");

    std::cout << "Shaders compiled" <<std::endl;
    auto unlitColorMat = Material(&unlitColor);
    //unlitColorMat.setVec4("color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));


    TextureCreateInfo texCreateInfo = {};
    texCreateInfo.generateMipmaps = true;
    texCreateInfo.magFilter = GL_LINEAR;
    texCreateInfo.minFilter = GL_LINEAR_MIPMAP_LINEAR;
    texCreateInfo.wrapMode = GL_CLAMP_TO_EDGE;
    auto texture = objMod.objectMaker.newTexture("Resources/Textures/tex.png", texCreateInfo);

    auto unlitTextureMat = Material(&unlitTexture);
    unlitTextureMat.setTexture("mainTex", texture);

    auto unlitInstancedMat = Material(&unlitInstanced);
    unlitInstancedMat.setTexture("mainTex", texture);

    TextureCreateInfo skyboxCreateInfo = {};
    skyboxCreateInfo.generateMipmaps = true;
    skyboxCreateInfo.magFilter = GL_LINEAR;
    skyboxCreateInfo.minFilter = GL_LINEAR;
    skyboxCreateInfo.wrapMode = GL_CLAMP_TO_EDGE;

    auto cubemap = objMod.objectMaker.newCubemap(skyboxCreateInfo, 
                                "Resources/Textures/skybox/nz.png", 
                                "Resources/Textures/skybox/nx.png", 
                                "Resources/Textures/skybox/px.png",
                                "Resources/Textures/skybox/pz.png",
                                "Resources/Textures/skybox/py.png",
                                "Resources/Textures/skybox/ny.png");

    auto skyboxMat = Material(&skyboxShader);
    skyboxMat.setCubemap("cubemap", cubemap);

    auto skinnedMat = Material(&skinnedShader);

    // objMod.objectMaker.newEntity(2);
    // {
    //     auto mr = objMod.objectMaker.newEmptyComponentForLastEntity<SkinnedMeshRenderer>();
    //         mr->material = &skinnedMat;
    //         mr->mesh = &objMod.assetReader.skinnedMeshes.find("Resources/Models/House Dancing.fbx/Alpha_Surface")->second;

    //     auto t = objMod.objectMaker.newEmptyComponentForLastEntity<Transform>();
    //         t->getLocalPositionModifiable() = { 0.0f, 0.0f, -25.0f };
    //         t->setParent(&sceneModule.rootNode);
    // }
    
    // objMod.objectMaker.newEntity(2);
    // {
    //     auto mr = objMod.objectMaker.newEmptyComponentForLastEntity<SkinnedMeshRenderer>();
    //         mr->material = &skinnedMat;
    //         mr->mesh = &objMod.assetReader.skinnedMeshes.find("Resources/Models/House Dancing.fbx/Alpha_Joints")->second;

    //     auto t = objMod.objectMaker.newEmptyComponentForLastEntity<Transform>();
    //         t->getLocalPositionModifiable() = { 0.0f, 0.0f, -25.0f };
    //         t->setParent(&sceneModule.rootNode);
    // }

    objMod.objectMaker.newEntity(2);
    {
        auto br = objMod.objectMaker.newEmptyComponentForLastEntity<BillboardRenderer>();
            br->material = &unlitInstancedMat;
        
        
        auto t = objMod.objectMaker.newEmptyComponentForLastEntity<Transform>();
            t->getLocalPositionModifiable() = { 10.0f, 0.0f, 0.0f };
            t->getLocalScaleModifiable() = { 10.0f, 10.0f, 10.0f };
            t->setParent(&sceneModule.rootNode);
    }

    objMod.objectMaker.newEntity(2);
    {
        auto br = objMod.objectMaker.newEmptyComponentForLastEntity<BillboardRenderer>();
            br->material = &unlitInstancedMat;
        
        auto t = objMod.objectMaker.newEmptyComponentForLastEntity<Transform>();
            t->getLocalPositionModifiable() = { 0.0f, 0.0f, 0.0f };
            t->getLocalScaleModifiable() = { 10.0f, 10.0f, 10.0f };
            t->setParent(&sceneModule.rootNode);
    }

    objMod.objectMaker.newEntity(2);
    {
        auto br = objMod.objectMaker.newEmptyComponentForLastEntity<BillboardRenderer>();
            br->material = &unlitInstancedMat;
        
        auto t = objMod.objectMaker.newEmptyComponentForLastEntity<Transform>();
            t->getLocalPositionModifiable() = { -10.0f, 0.0f, 0.0f };
            t->getLocalScaleModifiable() = { 10.0f, 10.0f, 10.0f };
            t->setParent(&sceneModule.rootNode);

    }

    objMod.objectMaker.newEntity(2);
    {
        auto br = objMod.objectMaker.newEmptyComponentForLastEntity<BillboardRenderer>();
            br->material = &unlitInstancedMat;
        
        auto t = objMod.objectMaker.newEmptyComponentForLastEntity<Transform>();
            t->getLocalPositionModifiable() = { 10.0f, 10.0f, 0.0f };
            t->getLocalScaleModifiable() = { 10.0f, 10.0f, 10.0f };
            t->setParent(&sceneModule.rootNode);
    }

    objMod.objectMaker.newEntity(2);
    {
        auto br = objMod.objectMaker.newEmptyComponentForLastEntity<BillboardRenderer>();
            br->material = &unlitInstancedMat;
        
        auto t = objMod.objectMaker.newEmptyComponentForLastEntity<Transform>();
            t->getLocalPositionModifiable() = { 0.0f, 10.0f, 0.0f };
            t->getLocalScaleModifiable() = { 10.0f, 10.0f, 10.0f };
            t->setParent(&sceneModule.rootNode);
    }

    objMod.objectMaker.newEntity(2);
    {
        auto br = objMod.objectMaker.newEmptyComponentForLastEntity<BillboardRenderer>();
            br->material = &unlitInstancedMat;
        
        auto t = objMod.objectMaker.newEmptyComponentForLastEntity<Transform>();
            t->getLocalPositionModifiable() = { -10.0f, 10.0f, 0.0f };
            t->getLocalScaleModifiable() = { 10.0f, 10.0f, 10.0f };
            t->setParent(&sceneModule.rootNode);
    }

    objMod.objectMaker.newEntity(4);
    {
        auto t = objMod.objectMaker.newEmptyComponentForLastEntity<Transform>();
            t->getLocalPositionModifiable() = { 0.5f, 0.0f, 10.0f };
            t->getLocalScaleModifiable() *= 10;
            t->setParent(&sceneModule.rootNode);

        auto c = objMod.objectMaker.newEmptyComponentForLastEntity<SphereCollider>();
            c->radius = 10;

        auto mr = objMod.objectMaker.newEmptyComponentForLastEntity<MeshRenderer>();
            mr->mesh = &objMod.assetReader.meshes.find("Resources/Models/unit_sphere.fbx/Sphere001")->second;
            mr->material = &unlitTextureMat;

        auto so1 = objMod.objectMaker.newEmptyComponentForLastEntity<AudioSource>();
            so1->getClipsModifiable().push_back("Resources/Audios/test.wav");
            so1->getIsRelativeModifiable() = false;
            so1->getIsLoopingModifiable() = true;
    }

    objMod.objectMaker.newEntity(3);
    {
        auto t = objMod.objectMaker.newEmptyComponentForLastEntity<Transform>();
            t->getLocalPositionModifiable()={-0.5f,0.0f,10.0f};
            t->getLocalScaleModifiable() *= 10;
            t->setParent(&sceneModule.rootNode);

        auto c = objMod.objectMaker.newEmptyComponentForLastEntity<SphereCollider>();
            c->radius = 10;

        auto mr = objMod.objectMaker.newEmptyComponentForLastEntity<MeshRenderer>();
            mr->mesh = &objMod.assetReader.meshes.find("Resources/Models/unit_sphere.fbx/Sphere001")->second;
            mr->material = &unlitColorMat;
    }

    objMod.objectMaker.newEntity(4);
    {
        auto c = objMod.objectMaker.newEmptyComponentForLastEntity<Camera>();
            c->farPlane = 1000.0f;
            c->nearPlane = 0.01f;
            c->fieldOfView = 80.0f;
            c->projectionMode = CameraProjection::Perspective;

        
        auto t = objMod.objectMaker.newEmptyComponentForLastEntity<Transform>();
            t->getLocalPositionModifiable() = glm::vec3(0.0f, 0.0f, 0.0f);
            t->setParent(&sceneModule.rootNode);

        auto li = objMod.objectMaker.newEmptyComponentForLastEntity<AudioListener>();
            li->getIsCurrentModifiable() = true;
            li->getGainModifiable() = 1.0f;
            li->getVelocityModifiable();
            li->getPositionModifiable();
            li->getAtModifiable();
            li->getUpModifiable();

        auto sc = objMod.objectMaker.newEmptyComponentForLastEntity<SphereCollider>();
            sc->type = Collider::Type::KINEMATIC;
            sc->radius = 5;
    }

    objMod.sceneWriter.saveScene();
}