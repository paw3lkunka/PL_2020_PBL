#include "Core.hpp"

#include "Components.inc"
#include "Systems.inc"

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

    // ! Scene loading
    //objectModule.readScene("Resources/Scenes/mainScene.json");

            #pragma region Data Loading
    
    objectModule.newModel("Resources/Models/unit_sphere.fbx");
    objectModule.newModel("Resources/Models/House Dancing.fbx");
    objectModule.newModel("Resources/Models/Cliffs.FBX");
    objectModule.newModel("Resources/Models/Left_bank.FBX");
    objectModule.newModel("Resources/Models/Right_bank.FBX");
    objectModule.newModel("Resources/Models/Riverbed.FBX");
    objectModule.newModel("Resources/Models/Water.FBX");
    objectModule.newModel("Resources/Models/Box.FBX");
    objectModule.newAudioClip("Resources/Audios/sample.wav");
    objectModule.newAudioClip("Resources/Audios/test.wav");
    

    auto unlitColor = objectModule.newShader("Resources/Shaders/UnlitColor/UnlitColor.vert", "Resources/Shaders/UnlitColor/UnlitColor.frag");

    auto unlitSkinned = objectModule.newShader("Resources/Shaders/UnlitSkinned/UnlitSkinned.vert", "Resources/Shaders/UnlitSkinned/UnlitSkinned.frag");
    

    auto unlitTexture = objectModule.newShader("Resources/Shaders/UnlitTexture/UnlitTexture.vert", "Resources/Shaders/UnlitTexture/UnlitTexture.frag");

    auto unlitInstanced = objectModule.newShader("Resources/Shaders/UnlitBillboardInstanced/UnlitBillboardInstanced.vert", "Resources/Shaders/UnlitBillboardInstanced/UnlitBillboardInstanced.frag");
    

    auto skyboxShader = objectModule.newShader("Resources/Shaders/SkyboxCubemap/SkyboxCubemap.vert", "Resources/Shaders/SkyboxCubemap/SkyboxCubemap.frag");


    std::cout << "Shaders compiled" << std::endl;
    Material* unlitColorMat = objectModule.newMaterial(unlitColor, "unlitColorMat");
    unlitColorMat->setVec4("color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

    Material* unlitSkinnedMat = objectModule.newMaterial(unlitSkinned, "unlitSkinnedMat");
    unlitSkinnedMat->setVec4("color", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

    TextureCreateInfo texCreateInfo = {};
    texCreateInfo.generateMipmaps = true;
    texCreateInfo.magFilter = GL_LINEAR;
    texCreateInfo.minFilter = GL_LINEAR_MIPMAP_LINEAR;
    texCreateInfo.wrapMode = GL_CLAMP_TO_EDGE;
    auto texture = objectModule.newTexture("Resources/Textures/tex.png", texCreateInfo);

    Material* unlitTextureMat = objectModule.newMaterial(unlitTexture, "unlitTextureMat");
    unlitTextureMat->setTexture("mainTex", texture);

    Material* unlitInstancedMat = objectModule.newMaterial(unlitInstanced, "unlitInstancedMat", true);
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

    Material* skyboxMat = objectModule.newMaterial(skyboxShader, "skyboxMat");
    skyboxMat->setCubemap("cubemap", cubemap);

    Material* waterMat = objectModule.newMaterial(unlitColor, "waterMat");
    waterMat->setVec4("color", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

    Material* cliffsMat = objectModule.newMaterial(unlitColor, "cliffsMat");
    cliffsMat->setVec4("color", glm::vec4(0.678f, 0.262f, 0.0f, 1.0f));

    Material* riverBedMat = objectModule.newMaterial(unlitColor, "riverBedMat");
    riverBedMat->setVec4("color", glm::vec4(0.407f, 0.2f, 0.070f, 1.0f));

    Material* riverBankMat = objectModule.newMaterial(unlitColor, "riverBankMat");
    riverBankMat->setVec4("color", glm::vec4(0.333f, 0.741f, 0.278f, 1.0f));

#pragma endregion

#pragma region Entities
    {
        auto entity = objectModule.getEntityFromName("Alpha_Surface");
        if(entity != nullptr)
        {
            auto mr = entity->getComponentPtr<MeshRenderer>();
                mr->material = unlitSkinnedMat;

            auto t = entity->getComponentPtr<Transform>();
                t->getLocalPositionModifiable() = { 0.0f, 0.0f, -25.0f };
        }
    }

    {
        auto entity = objectModule.getEntityFromName("Water");

        auto mr = entity->getComponentPtr<MeshRenderer>();
            mr->material = waterMat;

        auto t = entity->getComponentPtr<Transform>();
            t->getLocalPositionModifiable() = {0.0f, -30.0f, 0.0f};
            t->getLocalRotationModifiable() = eulerToQuaternion(glm::vec3(270, 0, 0));
            t->getLocalScaleModifiable() = {5, 5, 5};
    }

    {
        auto entity = objectModule.getEntityFromName("Cliffs");
        auto mr = entity->getComponentPtr<MeshRenderer>();
            mr->material = cliffsMat;

        auto t = entity->getComponentPtr<Transform>();
            t->getLocalPositionModifiable() = {0.0f, -30.0f, 0.0f};
            t->getLocalRotationModifiable() = eulerToQuaternion(glm::vec3(270, 0, 0));
            t->getLocalScaleModifiable() = {5, 5, 5};
    }

    {
        auto entity = objectModule.getEntityFromName("Left_bank");
        auto mr = entity->getComponentPtr<MeshRenderer>();
            mr->material = riverBankMat;

        auto t = entity->getComponentPtr<Transform>();
            t->getLocalPositionModifiable() = {0.0f, -30.0f, 0.0f};
            t->getLocalRotationModifiable() = eulerToQuaternion(glm::vec3(270, 0, 0));
            t->getLocalScaleModifiable() = {5, 5, 5};
    }

    {
        auto entity = objectModule.getEntityFromName("Right_bank");
        auto mr = entity->getComponentPtr<MeshRenderer>();
            mr->material = riverBankMat;

        auto t = entity->getComponentPtr<Transform>();
            t->getLocalPositionModifiable() = {0.0f, -30.0f, 0.0f};
            t->getLocalRotationModifiable() = eulerToQuaternion(glm::vec3(270, 0, 0));
            t->getLocalScaleModifiable() = {5, 5, 5};
            t->setParent(&sceneModule.rootNode);
    }

    {
        auto entity = objectModule.getEntityFromName("Riverbed");
        auto mr = entity->getComponentPtr<MeshRenderer>();
            mr->material = riverBedMat;

        auto t = entity->getComponentPtr<Transform>();
            t->getLocalPositionModifiable() = {0.0f, -30.0f, 0.0f};
            t->getLocalRotationModifiable() = eulerToQuaternion(glm::vec3(270, 0, 0));
            t->getLocalScaleModifiable() = {5, 5, 5};
    }

    {
        auto entity = objectModule.getEntityFromName("Alpha_Joints");
        auto mr = entity->getComponentPtr<MeshRenderer>();
            mr->material = unlitSkinnedMat;

        auto t = entity->getComponentPtr<Transform>();
            t->getLocalPositionModifiable() = { 0.0f, 0.0f, -25.0f };
    }

    {
        auto entity = objectModule.getEntityFromName("Sphere001");
        auto t = entity->getComponentPtr<Transform>();
            t->getLocalPositionModifiable() = {-10.5f,0.0f,10.0f};
            t->getLocalScaleModifiable() *= 10;
            t->setParent(&sceneModule.rootNode);

        auto mr = entity->getComponentPtr<MeshRenderer>();
            mr->material = unlitInstancedMat;

        auto c = objectModule.newEmptyComponent<SphereCollider>();
            c->radius = 10;

        entity->addComponent(c);
    }

    {
        auto entity = objectModule.getEntityFromName("Box001");
        auto t = entity->getComponentPtr<Transform>();
            t->getLocalPositionModifiable() = {0.0f, 0.0f, 0.0f};
            t->getLocalScaleModifiable() *= 10;
            t->setParent(&sceneModule.rootNode);

        auto mr = entity->getComponentPtr<MeshRenderer>();
            mr->material = unlitInstancedMat;
    }

    objectModule.newEntity(2, "sphereInstanced1");
    {
        auto t = objectModule.newEmptyComponentForLastEntity<Transform>();
            t->getLocalPositionModifiable() = { 20.5f, 0.0f, 10.0f };
            t->getLocalScaleModifiable() *= 10;
            t->setParent(&sceneModule.rootNode);

        auto mr = objectModule.newEmptyComponentForLastEntity<MeshRenderer>();
            mr->mesh = objectModule.getMeshCustomFromPath("Resources/Models/Box.FBX/Box001");
            mr->material = unlitInstancedMat;
    }

    objectModule.newEntity(2, "sphereInstanced2");
    {
        auto t = objectModule.newEmptyComponentForLastEntity<Transform>();
            t->getLocalPositionModifiable() = { -20.5f, 0.0f, 10.0f };
            t->getLocalScaleModifiable() *= 10;
            t->setParent(&sceneModule.rootNode);

        auto mr = objectModule.newEmptyComponentForLastEntity<MeshRenderer>();
            mr->mesh = objectModule.getMeshCustomFromPath("Resources/Models/Box.FBX/Box001");
            mr->material = unlitInstancedMat;
    }

    objectModule.newEntity(2, "sphereInstanced3");
    {
        auto t = objectModule.newEmptyComponentForLastEntity<Transform>();
            t->getLocalPositionModifiable() = { 30.5f, 0.0f, 10.0f };
            t->getLocalScaleModifiable() *= 10;
            t->setParent(&sceneModule.rootNode);

        auto mr = objectModule.newEmptyComponentForLastEntity<MeshRenderer>();
            mr->mesh = objectModule.getMeshCustomFromPath("Resources/Models/Box.FBX/Box001");
            mr->material = unlitInstancedMat;
    }

    objectModule.newEntity(2, "sphereInstanced4");
    {
        auto t = objectModule.newEmptyComponentForLastEntity<Transform>();
            t->getLocalPositionModifiable() = { -30.5f, 0.0f, 10.0f };
            t->getLocalScaleModifiable() *= 10;
            t->setParent(&sceneModule.rootNode);

        auto mr = objectModule.newEmptyComponentForLastEntity<MeshRenderer>();
            mr->mesh = objectModule.getMeshCustomFromPath("Resources/Models/Box.FBX/Box001");
            mr->material = unlitInstancedMat;
    }

    objectModule.newEntity(2, "sphereInstanced5");
    {
        auto t = objectModule.newEmptyComponentForLastEntity<Transform>();
            t->getLocalPositionModifiable() = { -30.5f, 10.0f, 10.0f };
            t->getLocalScaleModifiable() *= 10;
            t->setParent(&sceneModule.rootNode);

        auto mr = objectModule.newEmptyComponentForLastEntity<MeshRenderer>();
            mr->mesh = objectModule.getMeshCustomFromPath("Resources/Models/Box.FBX/Box001");
            mr->material = unlitInstancedMat;
    }

    objectModule.newEntity(2, "sphereInstanced6");
    {
        auto t = objectModule.newEmptyComponentForLastEntity<Transform>();
            t->getLocalPositionModifiable() = { 30.5f, 10.0f, 10.0f };
            t->getLocalScaleModifiable() *= 10;
            t->setParent(&sceneModule.rootNode);

        auto mr = objectModule.newEmptyComponentForLastEntity<MeshRenderer>();
            mr->mesh = objectModule.getMeshCustomFromPath("Resources/Models/Box.FBX/Box001");
            mr->material = unlitInstancedMat;
    }

    objectModule.newEntity(4, "Camera");
    {
        auto c = objectModule.newEmptyComponentForLastEntity<Camera>();
            c->farPlane = 10000.0f;
            c->nearPlane = 0.1f;
            c->fieldOfView = 80.0f;
            c->projectionMode = CameraProjection::Perspective;

        
        auto t = objectModule.newEmptyComponentForLastEntity<Transform>();
            t->getLocalPositionModifiable() = glm::vec3(0.0f, 0.0f, 0.0f);
            t->setParent(&sceneModule.rootNode);

        auto li = objectModule.newEmptyComponentForLastEntity<AudioListener>();
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


objectModule.newEntity(2, "sampleSound");
    {
        auto so3 = objectModule.newEmptyComponentForLastEntity<AudioSource>();
        so3->getListenersModifiable().push_back(objectModule.getEntityFromName("Camera")->getComponentPtr<AudioListener>());
        so3->getClipsModifiable().push_back("Resources/Audios/sample.wav");
        so3->getIsRelativeModifiable() = true;
        so3->getGainModifiable() = 0.1f;
        so3->getIsLoopingModifiable() = true;

        auto t = objectModule.newEmptyComponentForLastEntity<Transform>();
            t->getLocalPositionModifiable() = { 0.0f, 0.0f, 0.0f };
            t->setParent(&sceneModule.rootNode);
    }

    objectModule.newEntity(4, "sphereSound");
    {
        auto t = objectModule.newEmptyComponentForLastEntity<Transform>();
            t->getLocalPositionModifiable() = { 10.5f, 0.0f, 10.0f };
            t->getLocalScaleModifiable() *= 10;
            t->setParent(&sceneModule.rootNode);

        auto c = objectModule.newEmptyComponentForLastEntity<SphereCollider>();
            c->radius = 10;

        auto mr = objectModule.newEmptyComponentForLastEntity<MeshRenderer>();
            mr->mesh = objectModule.getMeshCustomFromPath("Resources/Models/unit_sphere.fbx/Sphere001");
            mr->material = unlitInstancedMat;

        auto so1 = objectModule.newEmptyComponentForLastEntity<AudioSource>();
            so1->getListenersModifiable().push_back(objectModule.getEntityFromName("Camera")->getComponentPtr<AudioListener>());
            so1->getClipsModifiable().push_back("Resources/Audios/test.wav");
            so1->getGainModifiable() = 100.5f;
            so1->getIsRelativeModifiable() = false;
            so1->getIsLoopingModifiable() = true;
    }


    if (updateScene)
    {
        
    }

#pragma region Renderer

    // ! ----- Renderer initialization block -----
    RendererModuleCreateInfo rendererCreateInfo = {};
    rendererCreateInfo.clearColor = glm::vec3(0.0f, 1.0f, 0.0f);
    rendererCreateInfo.clearFlags = GL_DEPTH_BUFFER_BIT;
    rendererCreateInfo.cullFace = true;
    rendererCreateInfo.cullFaceMode = GL_BACK;
    rendererCreateInfo.cullFrontFace = GL_CCW;
    rendererCreateInfo.depthTest = true;
    rendererCreateInfo.wireframeMode = false;
    rendererModule.initialize(window, rendererCreateInfo, objectModule.getMaterialFromName("skyboxMat"));
    
    messageBus.addReceiver( &rendererModule );
#pragma endregion

    gameSystemsModule.addSystem(&rendererSystem);
    gameSystemsModule.addSystem(&cameraControlSystem);
    gameSystemsModule.addSystem(&collisionDetectionSystem);
    //gameSystemsModule.addSystem(&gravitySystem);
    //gameSystemsModule.addSystem(&kinematicSystem);
    gameSystemsModule.addSystem(&skeletonSystem);

#pragma region AudioModule demo - initialization
    
    audioModule.init();

    gameSystemsModule.addSystem(&audioListenerSystem);
    gameSystemsModule.addSystem(&audioSourceSystem);
    
#pragma endregion

#pragma region Camera
    // ! Finding main camera
    CameraSystem::setAsMain(objectModule.getEntityFromName("Camera"));

    gameSystemsModule.addSystem(&cameraSystem);

#pragma endregion

    gameSystemsModule.entities = objectModule.getEntitiesVector();

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
        messageBus.sendMessage( Message(Event::AUDIO_SOURCE_PLAY, objectModule.getEntityFromName("sampleSound")->getComponentPtr<AudioSource>()) );
        messageBus.sendMessage( Message(Event::AUDIO_SOURCE_PLAY, objectModule.getEntityFromName("sphereSound")->getComponentPtr<AudioSource>()));
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
    //HACK: scene saving- uncomment when changing something in scene
    objectModule.saveScene("../resources/Scenes/savedScene.json");

    audioModule.cleanup();

	glfwDestroyWindow(window);
	glfwTerminate();
    
    std::cout << "App freed resources properly." << std::endl;
}

void Core::close()
{
    glfwSetWindowShouldClose(window,true);
}

CameraSystem Core::cameraSystem;
CameraControlSystem Core::cameraControlSystem;
AudioSourceSystem Core::audioSourceSystem;
AudioListenerSystem Core::audioListenerSystem;
MeshRendererSystem Core::rendererSystem;
CollisionDetectionSystem Core::collisionDetectionSystem;
GravitySystem Core::gravitySystem;
KinematicSystem Core::kinematicSystem;
SkeletonSystem Core::skeletonSystem;