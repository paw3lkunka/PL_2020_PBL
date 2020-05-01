#include "Core.hpp"
#include "Message.inl"
#include "Event.hpp"
#include "Cubemap.hpp"
#include "FileStructures.inl"

// * C++ std lib
#include <vector>
#include <iostream>
#include <utility>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

Core* Core::instance = nullptr;
int Core::windowWidth = INIT_WINDOW_WIDTH;
int Core::windowHeight = INIT_WINDOW_HEIGHT;

Core& GetCore()
{
    return *(Core::instance);
}

void InfoLog(std::string log)
{ 
    Core::instance->messageBus.sendMessage(Message(Event::DEBUG_INFO_LOG, log.c_str()));
}

void WarningLog(const char* log)
{
    Core::instance->messageBus.sendMessage(Message(Event::DEBUG_WARNING_LOG, log));
}

void ErrorLog(const char* log)
{   
    Core::instance->messageBus.sendMessage(Message(Event::DEBUG_ERROR_LOG, log));
}

glm::quat eulerToQuaternion(glm::vec3 eulerAngles)
{
    glm::mat4 temp = glm::mat4(1);
    temp = glm::rotate(temp, glm::radians(eulerAngles.x), glm::vec3(1.0, 0.0, 0.0));
    temp = glm::rotate(temp, glm::radians(eulerAngles.y), glm::vec3(0.0, 1.0, 0.0));
    temp = glm::rotate(temp, glm::radians(eulerAngles.z), glm::vec3(0.0, 0.0, 1.0));
    glm::quat quatFinal = glm::quat(temp);
    return quatFinal;
}

int Core::init()
{
    if( instance != nullptr )
    {
		std::cerr << "Core already initialized" << std::endl;
		return instance == this ? 3 : 4;
    }
    instance = this;
    
    std::cout << "Henlo!" << std::endl;
    //TODO: GLFW Error callback
    
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(windowWidth, windowHeight, "PBL", NULL, NULL);
    if (window == NULL)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return 2;
	}

    glViewport(0,0,windowWidth,windowHeight);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    
    //Initializing Modules, and adding connecting to MB
    inputModule.initialize(window);

    messageBus.addReceiver( &inputModule );    
    messageBus.addReceiver( &consoleModule );
    messageBus.addReceiver( &gameSystemsModule );
    messageBus.addReceiver( &audioModule );
    messageBus.addReceiver( &objectModule );
    messageBus.addReceiver( &tmpExit );

    objectModule.readScene("scene.json");

#pragma region Data Loading
    
    objectModule.newModel("Resources/Models/Test.fbx", FileType::MESH);
    objectModule.newModel("Resources/Models/unit_sphere.fbx", FileType::MESH);
    objectModule.newModel("Resources/Models/House Dancing.fbx", FileType::MESH);
    objectModule.newModel("Resources/Models/Cliffs.FBX", FileType::MESH);
    objectModule.newModel("Resources/Models/Left_bank.FBX", FileType::MESH);
    objectModule.newModel("Resources/Models/Right_bank.FBX", FileType::MESH);
    objectModule.newModel("Resources/Models/Riverbed.FBX", FileType::MESH);
    objectModule.newModel("Resources/Models/Water.FBX", FileType::MESH);
    objectModule.newAudioClip("Resources/Audios/sample.wav");
    objectModule.newAudioClip("Resources/Audios/test.wav");
    

    auto unlitColor = objectModule.newShader("Resources/Shaders/UnlitColor/UnlitColor.vert", "Resources/Shaders/UnlitColor/UnlitColor.frag");
    

    auto unlitTexture = objectModule.newShader("Resources/Shaders/UnlitTexture/UnlitTexture.vert", "Resources/Shaders/UnlitTexture/UnlitTexture.frag");

    auto unlitInstanced = objectModule.newShader("Resources/Shaders/UnlitBillboardInstanced/UnlitBillboardInstanced.vert", "Resources/Shaders/UnlitBillboardInstanced/UnlitBillboardInstanced.frag");
    

    auto skyboxShader = objectModule.newShader("Resources/Shaders/SkyboxCubemap/SkyboxCubemap.vert", "Resources/Shaders/SkyboxCubemap/SkyboxCubemap.frag");

    //auto skinnedShader = objectModule.newShader("Resources/Shaders/UnlitSkinned/UnlitSkinned.vert", "Resources/Shaders/UnlitSkinned/UnlitSkinned.frag");

    std::cout << "Shaders compiled" <<std::endl;
    Material* unlitColorMat = objectModule.newMaterial(unlitColor);
    unlitColorMat->setVec4("color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));


    TextureCreateInfo texCreateInfo = {};
    texCreateInfo.generateMipmaps = true;
    texCreateInfo.magFilter = GL_LINEAR;
    texCreateInfo.minFilter = GL_LINEAR_MIPMAP_LINEAR;
    texCreateInfo.wrapMode = GL_CLAMP_TO_EDGE;
    auto texture = objectModule.newTexture("Resources/Textures/tex.png", texCreateInfo);

    Material* unlitTextureMat = objectModule.newMaterial(unlitTexture);
    unlitTextureMat->setTexture("mainTex", texture);

    Material* unlitInstancedMat = objectModule.newMaterial(unlitInstanced);
    unlitInstancedMat->setTexture("mainTex", texture);

    TextureCreateInfo skyboxCreateInfo = {};
    skyboxCreateInfo.generateMipmaps = true;
    skyboxCreateInfo.magFilter = GL_LINEAR;
    skyboxCreateInfo.minFilter = GL_LINEAR;
    skyboxCreateInfo.wrapMode = GL_CLAMP_TO_EDGE;

    auto cubemap = objectModule.newCubemap(skyboxCreateInfo, 
                                "Resources/Textures/skybox/nz.png", 
                                "Resources/Textures/skybox/nx.png", 
                                "Resources/Textures/skybox/px.png",
                                "Resources/Textures/skybox/pz.png",
                                "Resources/Textures/skybox/py.png",
                                "Resources/Textures/skybox/ny.png");

    Material* skyboxMat = objectModule.newMaterial(skyboxShader);
    skyboxMat->setCubemap("cubemap", cubemap);

    //Material* skinnedMat = objectModule.newMaterial(skinnedShader);

    Material* waterMat = objectModule.newMaterial(unlitColor);
    waterMat->setVec4("color", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

    Material* cliffsMat = objectModule.newMaterial(unlitColor);
    cliffsMat->setVec4("color", glm::vec4(0.678f, 0.262f, 0.0f, 1.0f));

    Material* riverBedMat = objectModule.newMaterial(unlitColor);
    riverBedMat->setVec4("color", glm::vec4(0.407f, 0.2f, 0.070f, 1.0f));

    Material* riverBankMat = objectModule.newMaterial(unlitColor);
    riverBankMat->setVec4("color", glm::vec4(0.333f, 0.741f, 0.278f, 1.0f));

#pragma endregion

#pragma region Renderer

    // // ! ----- Renderer initialization block -----
    RendererModuleCreateInfo rendererCreateInfo = {};
    rendererCreateInfo.clearColor = glm::vec3(0.0f, 1.0f, 0.0f);
    rendererCreateInfo.clearFlags = GL_DEPTH_BUFFER_BIT;
    rendererCreateInfo.cullFace = true;
    rendererCreateInfo.cullFaceMode = GL_BACK;
    rendererCreateInfo.cullFrontFace = GL_CCW;
    rendererCreateInfo.depthTest = true;
    rendererCreateInfo.wireframeMode = false;
    rendererModule.initialize(window, rendererCreateInfo, skyboxMat);
    
    messageBus.addReceiver( &rendererModule );

#pragma endregion

#pragma region Entities
    objectModule.newEntity(2);
    {
        auto mr = objectModule.newEmptyComponentForLastEntity<SkinnedMeshRenderer>();
            mr->material = unlitColorMat;
            mr->mesh = objectModule.getMeshCustomFromPath("Resources/Models/House Dancing.fbx/Alpha_Surface");

        auto t = objectModule.newEmptyComponentForLastEntity<Transform>();
            t->getLocalPositionModifiable() = { 0.0f, 0.0f, -25.0f };
            t->setParent(&sceneModule.rootNode);
    }

    objectModule.newEntity(2);
    {
        auto mr = objectModule.newEmptyComponentForLastEntity<MeshRenderer>();
            mr->material = waterMat;
            mr->mesh = objectModule.getMeshCustomFromPath("Resources/Models/Water.FBX/Water");

        auto t = objectModule.newEmptyComponentForLastEntity<Transform>();
            t->getLocalPositionModifiable() = {0.0f, -30.0f, 0.0f};
            t->getLocalRotationModifiable() = eulerToQuaternion(glm::vec3(270, 0, 0));
            t->getLocalScaleModifiable() = {5, 5, 5};
            t->setParent(&sceneModule.rootNode);
    }

    objectModule.newEntity(2);
    {
        auto mr = objectModule.newEmptyComponentForLastEntity<MeshRenderer>();
            mr->material = cliffsMat;
            mr->mesh = objectModule.getMeshCustomFromPath("Resources/Models/Cliffs.FBX/Cliffs");

        auto t = objectModule.newEmptyComponentForLastEntity<Transform>();
            t->getLocalPositionModifiable() = {0.0f, -30.0f, 0.0f};
            t->getLocalRotationModifiable() = eulerToQuaternion(glm::vec3(270, 0, 0));
            t->getLocalScaleModifiable() = {5, 5, 5};
            t->setParent(&sceneModule.rootNode);
    }

    objectModule.newEntity(2);
    {
        auto mr = objectModule.newEmptyComponentForLastEntity<MeshRenderer>();
            mr->material = riverBankMat;
            mr->mesh = objectModule.getMeshCustomFromPath("Resources/Models/Left_bank.FBX/Left_bank");

        auto t = objectModule.newEmptyComponentForLastEntity<Transform>();
            t->getLocalPositionModifiable() = {0.0f, -30.0f, 0.0f};
            t->getLocalRotationModifiable() = eulerToQuaternion(glm::vec3(270, 0, 0));
            t->getLocalScaleModifiable() = {5, 5, 5};
            t->setParent(&sceneModule.rootNode);
    }

    objectModule.newEntity(2);
    {
        auto mr = objectModule.newEmptyComponentForLastEntity<MeshRenderer>();
            mr->material = riverBankMat;
            mr->mesh = objectModule.getMeshCustomFromPath("Resources/Models/Right_bank.FBX/Right_bank");

        auto t = objectModule.newEmptyComponentForLastEntity<Transform>();
            t->getLocalPositionModifiable() = {0.0f, -30.0f, 0.0f};
            t->getLocalRotationModifiable() = eulerToQuaternion(glm::vec3(270, 0, 0));
            t->getLocalScaleModifiable() = {5, 5, 5};
            t->setParent(&sceneModule.rootNode);
    }

    objectModule.newEntity(2);
    {
        auto mr = objectModule.newEmptyComponentForLastEntity<MeshRenderer>();
            mr->material = riverBedMat;
            mr->mesh = objectModule.getMeshCustomFromPath("Resources/Models/Riverbed.FBX/Riverbed");

        auto t = objectModule.newEmptyComponentForLastEntity<Transform>();
            t->getLocalPositionModifiable() = {0.0f, -30.0f, 0.0f};
            t->getLocalRotationModifiable() = eulerToQuaternion(glm::vec3(270, 0, 0));
            t->getLocalScaleModifiable() = {5, 5, 5};
            t->setParent(&sceneModule.rootNode);
    }
    
    objectModule.newEntity(2);
    {
        auto mr = objectModule.newEmptyComponentForLastEntity<SkinnedMeshRenderer>();
            mr->material = unlitColorMat;
            mr->mesh = objectModule.getMeshCustomFromPath("Resources/Models/House Dancing.fbx/Alpha_Joints");

        auto t = objectModule.newEmptyComponentForLastEntity<Transform>();
            t->getLocalPositionModifiable() = { 0.0f, 0.0f, -25.0f };
            t->setParent(&sceneModule.rootNode);
    }

    objectModule.newEntity(2);
    {
        auto br = objectModule.newEmptyComponentForLastEntity<BillboardRenderer>();
            br->material = unlitInstancedMat;
        
        
        auto t = objectModule.newEmptyComponentForLastEntity<Transform>();
            t->getLocalPositionModifiable() = { 10.0f, 0.0f, 0.0f };
            t->getLocalScaleModifiable() = { 10.0f, 10.0f, 10.0f };
            t->setParent(&sceneModule.rootNode);
    }

    objectModule.newEntity(2);
    {
        auto br = objectModule.newEmptyComponentForLastEntity<BillboardRenderer>();
            br->material = unlitInstancedMat;
        
        auto t = objectModule.newEmptyComponentForLastEntity<Transform>();
            t->getLocalPositionModifiable() = { 0.0f, 0.0f, 0.0f };
            t->getLocalScaleModifiable() = { 10.0f, 10.0f, 10.0f };
            t->setParent(&sceneModule.rootNode);
    }

    objectModule.newEntity(2);
    {
        auto br = objectModule.newEmptyComponentForLastEntity<BillboardRenderer>();
            br->material = unlitInstancedMat;
        
        auto t = objectModule.newEmptyComponentForLastEntity<Transform>();
            t->getLocalPositionModifiable() = { -10.0f, 0.0f, 0.0f };
            t->getLocalScaleModifiable() = { 10.0f, 10.0f, 10.0f };
            t->setParent(&sceneModule.rootNode);

    }

    objectModule.newEntity(2);
    {
        auto br = objectModule.newEmptyComponentForLastEntity<BillboardRenderer>();
            br->material = unlitInstancedMat;
        
        auto t = objectModule.newEmptyComponentForLastEntity<Transform>();
            t->getLocalPositionModifiable() = { 10.0f, 10.0f, 0.0f };
            t->getLocalScaleModifiable() = { 10.0f, 10.0f, 10.0f };
            t->setParent(&sceneModule.rootNode);
    }

    objectModule.newEntity(2);
    {
        auto br = objectModule.newEmptyComponentForLastEntity<BillboardRenderer>();
            br->material = unlitInstancedMat;
        
        auto t = objectModule.newEmptyComponentForLastEntity<Transform>();
            t->getLocalPositionModifiable() = { 0.0f, 10.0f, 0.0f };
            t->getLocalScaleModifiable() = { 10.0f, 10.0f, 10.0f };
            t->setParent(&sceneModule.rootNode);
    }

    objectModule.newEntity(2);
    {
        auto br = objectModule.newEmptyComponentForLastEntity<BillboardRenderer>();
            br->material = unlitInstancedMat;
        
        auto t = objectModule.newEmptyComponentForLastEntity<Transform>();
            t->getLocalPositionModifiable() = { -10.0f, 10.0f, 0.0f };
            t->getLocalScaleModifiable() = { 10.0f, 10.0f, 10.0f };
            t->setParent(&sceneModule.rootNode);
    }

    objectModule.newEntity(3);
    {
        auto t = objectModule.newEmptyComponentForLastEntity<Transform>();
            t->getLocalPositionModifiable()={-10.5f,0.0f,10.0f};
            t->getLocalScaleModifiable() *= 10;
            t->setParent(&sceneModule.rootNode);

        auto c = objectModule.newEmptyComponentForLastEntity<SphereCollider>();
            c->radius = 10;

        auto mr = objectModule.newEmptyComponentForLastEntity<MeshRenderer>();
            mr->mesh = objectModule.getMeshCustomFromPath("Resources/Models/unit_sphere.fbx/Sphere001");
            mr->material = unlitColorMat;
    }

    objectModule.newEntity(4);
    {
        auto c = objectModule.newEmptyComponentForLastEntity<Camera>();
            c->farPlane = 1000.0f;
            c->nearPlane = 0.01f;
            c->fieldOfView = 80.0f;
            c->projectionMode = CameraProjection::Perspective;

        
        auto t = objectModule.newEmptyComponentForLastEntity<Transform>();
            t->getLocalPositionModifiable() = glm::vec3(0.0f, 0.0f, 0.0f);
            t->setParent(&sceneModule.rootNode);

        li = objectModule.newEmptyComponentForLastEntity<AudioListener>();
            li->getIsCurrentModifiable() = true;
            li->getGainModifiable() = 1.0f;
            li->getVelocityModifiable();
            li->getPositionModifiable();
            li->getAtModifiable();
            li->getUpModifiable();

        auto sc = objectModule.newEmptyComponentForLastEntity<SphereCollider>();
            sc->type = Collider::Type::KINEMATIC;
            sc->radius = 5;
    }

#pragma endregion


    gameSystemsModule.addSystem(&rendererSystem);
    gameSystemsModule.addSystem(&cameraControlSystem);
    gameSystemsModule.addSystem(&billboardSystem);
    //gameSystemsModule.addSystem(&collisionDetectionSystem);
    gameSystemsModule.addSystem(&boneSystem);
    //gameSystemsModule.addSystem(&skinnedMeshRendererSystem);


#pragma region Camera

    CameraSystem::setAsMain(&objectModule.getEntitiesVector()->back());

    gameSystemsModule.addSystem(&cameraSystem);

#pragma endregion

#pragma region AudioModule demo - initialization
    
    audioModule.init();
    
    // objectModule.newEntity(2);
    // {
    //     so1 = objectModule.newEmptyComponentForLastEntity<AudioSource>();
    //         so1->getListenersModifiable().push_back(li);
    //         so1->getClipsModifiable().push_back("Resources/Audios/test.wav");
    //         so1->getIsRelativeModifiable() = false;
    //         so1->getIsLoopingModifiable() = true;

    //     auto t = objectModule.newEmptyComponentForLastEntity<Transform>();
    //         t->getLocalPositionModifiable() = { 0.0f, 0.0f, 0.0f };
    //         t->setParent(&sceneModule.rootNode);
    // }


    /*
    objectModule.NewEntity(2);
    {
        so2 = objectModule.NewComponent<AudioSource>();
            so2->getListenersModifiable().push_back(li);
            so2->getClipsModifiable().push_back("Resources/Audios/test.wav");
            so2->getIsRelativeModifiable() = false;
            so2->getIsLoopingModifiable() = true;

        auto t = objectModule.NewComponent<Transform>();
            t->getLocalPositionModifiable() = { 0.0f, 0.0f, 0.0f };
            t->setParent(&sceneModule.rootNode);
    }
    */

    objectModule.newEntity(2);
    {
        so3 = objectModule.newEmptyComponentForLastEntity<AudioSource>();
        so3->getListenersModifiable().push_back(li);
        so3->getClipsModifiable().push_back("Resources/Audios/sample.wav");
        so3->getIsRelativeModifiable() = true;
        so3->getGainModifiable() = 0.1f;
        so3->getIsLoopingModifiable() = true;

        auto t = objectModule.newEmptyComponentForLastEntity<Transform>();
            t->getLocalPositionModifiable() = { 0.0f, 0.0f, 0.0f };
            t->setParent(&sceneModule.rootNode);
    }

    objectModule.newEntity(4);
    {
        auto t = objectModule.newEmptyComponentForLastEntity<Transform>();
            t->getLocalPositionModifiable() = { 10.5f, 0.0f, 10.0f };
            t->getLocalScaleModifiable() *= 10;
            t->setParent(&sceneModule.rootNode);

        auto c = objectModule.newEmptyComponentForLastEntity<SphereCollider>();
            c->radius = 10;

        auto mr = objectModule.newEmptyComponentForLastEntity<MeshRenderer>();
            mr->mesh = objectModule.getMeshCustomFromPath("Resources/Models/unit_sphere.fbx/Sphere001");
            mr->material = unlitTextureMat;

        so1 = objectModule.newEmptyComponentForLastEntity<AudioSource>();
            so1->getListenersModifiable().push_back(li);
            so1->getClipsModifiable().push_back("Resources/Audios/test.wav");
            so1->getGainModifiable() = 100.5f;
            so1->getIsRelativeModifiable() = false;
            so1->getIsLoopingModifiable() = true;
    }

    gameSystemsModule.addSystem(&audioListenerSystem);
    gameSystemsModule.addSystem(&audioSourceSystem);
    
#pragma endregion

    gameSystemsModule.entities = objectModule.getEntitiesVector();

    objectModule.saveScene("saved.json");
    // Everything is ok.
    return 0;
}

int Core::mainLoop()
{
    double previousFrameStart = glfwGetTime();
    //HACK temporary solution, should be 0 n start
    double lag = FIXED_TIME_STEP;

    // ! ----- START SYSTEM FUNCTION -----
    
        gameSystemsModule.run(System::START);

#pragma region AudioModule demo
        messageBus.sendMessage( Message(Event::AUDIO_SOURCE_PLAY, so1) );
        //messageBus.sendMessage( Message(Event::AUDIO_SOURCE_PLAY, so2) );
        messageBus.sendMessage( Message(Event::AUDIO_SOURCE_PLAY, so3) );
#pragma endregion

    // * ===== Game loop ===================================================

    sceneModule.updateTransforms();

    //Main loop
    while (!glfwWindowShouldClose(window))
    {

        // ? +++++ FIXED UPDATE TIME MANAGEMENT +++++

            double currentFrameStart = glfwGetTime();
            double lastFrameTime = currentFrameStart - previousFrameStart;

            lag += lastFrameTime;
            previousFrameStart = currentFrameStart;

        // ? +++++ GLFW EVENTS AND INPUT +++++

            glfwPollEvents();
            inputModule.captureControllersInput();
        
        // ? +++++ FIXED UPDATE LOOP +++++

        while(lag >= FIXED_TIME_STEP)
        {
            // Read message bus messages
            messageBus.notify();

            // ! ----- FIXED UPDATE FUNCTION -----
            
            gameSystemsModule.run(System::FIXED);

            // Traverse the scene graph and update transforms
            sceneModule.updateTransforms();

            // Decrease the lag by fixed step
            lag -= FIXED_TIME_STEP;
        }

        // ! ----- FRAME UPDATE FUNCTION -----

        gameSystemsModule.run(System::FRAME);
        // Read message bus before rendering
        // TODO: Should transform update be here also?
        messageBus.notify();

        // ? +++++ RENDER CURRENT FRAME +++++

        rendererModule.render();
        // Clear input flags at the end of frame
        inputModule.clearFlags();
    }    

    return 0;
}

void Core::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    windowWidth = width;
    windowHeight = height;
    GetCore().getMessageBus().sendMessage(Message(Event::WINDOW_RESIZED, glm::ivec2(width, height)));
    GetCore().getMessageBus().notify();
}

MessageBus& Core::getMessageBus()
{
    return messageBus;
}

void Core::cleanup()
{
    audioModule.cleanup();

	glfwDestroyWindow(window);
	glfwTerminate();
    
    std::cout << "App freed resources properly." << std::endl;
}

void Core::close()
{
    glfwSetWindowShouldClose(window,true);
}
