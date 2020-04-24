#include "Core.hpp"

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
    messageBus.addReceiver( &resourceModule );
    messageBus.addReceiver( &tmpExit );

#pragma region Data Loading

    objectModule.resourceModulePtr = &resourceModule;
    resourceModule.objectModulePtr = &objectModule;
    
    FileSystemData fsData;
    fsData.path = "Resources/Audios/sample.wav";
    fsData.typeOfFile = FileType::AUDIO;

    FileSystemData unlitColorVertexShaderData;
    unlitColorVertexShaderData.path = "Resources/Shaders/UnlitColor/UnlitColor.vert";
    unlitColorVertexShaderData.typeOfFile = FileType::SHADER;

    FileSystemData unlitColorFragmentShaderData;
    unlitColorFragmentShaderData.path = "Resources/Shaders/UnlitColor/UnlitColor.frag";
    unlitColorFragmentShaderData.typeOfFile = FileType::SHADER;

    FileSystemData unlitTextureVertexShaderData;
    unlitTextureVertexShaderData.path = "Resources/Shaders/UnlitTexture/UnlitTexture.vert";
    unlitTextureVertexShaderData.typeOfFile = FileType::SHADER;

    FileSystemData unlitTextureFragmentShaderData;
    unlitTextureFragmentShaderData.path = "Resources/Shaders/UnlitTexture/UnlitTexture.frag";
    unlitTextureFragmentShaderData.typeOfFile = FileType::SHADER;

    FileSystemData unlitInstancedVertexShaderData;
    unlitInstancedVertexShaderData.path = "Resources/Shaders/UnlitBillboardInstanced/UnlitBillboardInstanced.vert";
    unlitInstancedVertexShaderData.typeOfFile = FileType::SHADER;

    FileSystemData unlitInstancedFragmentShaderData;
    unlitInstancedFragmentShaderData.path = "Resources/Shaders/UnlitBillboardInstanced/UnlitBillboardInstanced.frag";
    unlitInstancedFragmentShaderData.typeOfFile = FileType::SHADER;

    FileSystemData skyboxCubemapVertexShaderData;
    skyboxCubemapVertexShaderData.path = "Resources/Shaders/SkyboxCubemap/SkyboxCubemap.vert";
    skyboxCubemapVertexShaderData.typeOfFile = FileType::SHADER;

    FileSystemData skyboxCubemapFragmentShaderData;
    skyboxCubemapFragmentShaderData.path = "Resources/Shaders/SkyboxCubemap/SkyboxCubemap.frag";
    skyboxCubemapFragmentShaderData.typeOfFile = FileType::SHADER;

    FileSystemData testModel;
    testModel.path = "Resources/Models/Test.fbx";
    testModel.typeOfFile = FileType::MESH;

    FileSystemData animModel;
    animModel.path = "Resources/Models/House Dancing.fbx";
    // ! SEGFAULT - uncomment if there will be SinnedMeshRenderer
    //animModel.typeOfFile = FileType::SKINNEDMESH;
    animModel.typeOfFile = FileType::MESH;

    FileSystemData testTexture;
    testTexture.path = "Resources/Textures/tex.png";
    testTexture.typeOfFile = FileType::TEXTURE;

    FileSystemData skyNX;
    skyNX.path = "Resources/Textures/skybox/nx.png";
    skyNX.typeOfFile = FileType::TEXTURE;

    FileSystemData skyNY;
    skyNY.path = "Resources/Textures/skybox/ny.png";
    skyNY.typeOfFile = FileType::TEXTURE;

    FileSystemData skyNZ;
    skyNZ.path = "Resources/Textures/skybox/nz.png";
    skyNZ.typeOfFile = FileType::TEXTURE;

    FileSystemData skyPX;
    skyPX.path = "Resources/Textures/skybox/px.png";
    skyPX.typeOfFile = FileType::TEXTURE;

    FileSystemData skyPY;
    skyPY.path = "Resources/Textures/skybox/py.png";
    skyPY.typeOfFile = FileType::TEXTURE;

    FileSystemData skyPZ;
    skyPZ.path = "Resources/Textures/skybox/pz.png";
    skyPZ.typeOfFile = FileType::TEXTURE;

    FileSystemData sphere ={
        .path = "Resources/Models/unit_sphere.fbx",
        .typeOfFile = FileType::MESH
    };

    getMessageBus().sendMessage(Message(Event::LOAD_FILE, unlitColorVertexShaderData));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, unlitColorFragmentShaderData));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, unlitTextureVertexShaderData));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, unlitTextureFragmentShaderData));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, unlitInstancedVertexShaderData));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, unlitInstancedFragmentShaderData));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, skyboxCubemapVertexShaderData));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, skyboxCubemapFragmentShaderData));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, testModel));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, animModel));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, testTexture));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, skyNX));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, skyNY));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, skyNZ));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, skyPX));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, skyPY));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, skyPZ));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, fsData));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, skyPZ));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, sphere));
    getMessageBus().notify();

    getMessageBus().sendMessage(Message(Event::QUERY_TEXTURE_DATA, "Resources/Textures/tex.png"));
    getMessageBus().notify();

#pragma endregion

#pragma region Renderer
    unlitColor = Shader(resourceModule.shaders.find("Resources/Shaders/UnlitColor/UnlitColor.vert")->second.c_str(),
                        resourceModule.shaders.find("Resources/Shaders/UnlitColor/UnlitColor.frag")->second.c_str());
    unlitTexture = Shader(  resourceModule.shaders.find("Resources/Shaders/UnlitTexture/UnlitTexture.vert")->second.c_str(),
                            resourceModule.shaders.find("Resources/Shaders/UnlitTexture/UnlitTexture.frag")->second.c_str());
    unlitInstanced = Shader(resourceModule.shaders.find("Resources/Shaders/UnlitBillboardInstanced/UnlitBillboardInstanced.vert")->second.c_str(),
                            resourceModule.shaders.find("Resources/Shaders/UnlitBillboardInstanced/UnlitBillboardInstanced.frag")->second.c_str());
    skyboxShader = Shader(  resourceModule.shaders.find("Resources/Shaders/SkyboxCubemap/SkyboxCubemap.vert")->second.c_str(),
                            resourceModule.shaders.find("Resources/Shaders/SkyboxCubemap/SkyboxCubemap.frag")->second.c_str());
    unlitColorMat = Material(&unlitColor);
    unlitColorMat.setVec4("color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

    TextureData texData = resourceModule.textures.find("Resources/Textures/tex.png")->second;

    TextureCreateInfo texCreateInfo = {};
    texCreateInfo.format = GL_RGBA;
    texCreateInfo.generateMipmaps = true;
    texCreateInfo.magFilter = GL_LINEAR;
    texCreateInfo.minFilter = GL_LINEAR_MIPMAP_LINEAR;
    texCreateInfo.wrapMode = GL_CLAMP_TO_EDGE;
    texCreateInfo.width = texData.width;
    texCreateInfo.height = texData.height;
    Texture texture(texData.data, texCreateInfo);

    unlitTextureMat = Material(&unlitTexture);
    unlitTextureMat.setTexture("mainTex", texture);

    unlitInstancedMat = Material(&unlitInstanced);
    unlitInstancedMat.setTexture("mainTex", texture);

    TextureData skyboxNX = resourceModule.textures.find("Resources/Textures/skybox/nx.png")->second;
    TextureData skyboxNY = resourceModule.textures.find("Resources/Textures/skybox/ny.png")->second;
    TextureData skyboxNZ = resourceModule.textures.find("Resources/Textures/skybox/nz.png")->second;
    TextureData skyboxPX = resourceModule.textures.find("Resources/Textures/skybox/px.png")->second;
    TextureData skyboxPY = resourceModule.textures.find("Resources/Textures/skybox/py.png")->second;
    TextureData skyboxPZ = resourceModule.textures.find("Resources/Textures/skybox/pz.png")->second;
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

    skyboxMat = Material(&skyboxShader);
    skyboxMat.setCubemap("cubemap", cubemap);

    // ! ----- Renderer initialization block -----
    RendererModuleCreateInfo rendererCreateInfo = {};
    rendererCreateInfo.clearColor = glm::vec3(0.0f, 1.0f, 0.0f);
    rendererCreateInfo.clearFlags = GL_DEPTH_BUFFER_BIT;
    rendererCreateInfo.cullFace = true;
    rendererCreateInfo.cullFaceMode = GL_BACK;
    rendererCreateInfo.cullFrontFace = GL_CCW;
    rendererCreateInfo.depthTest = true;
    rendererCreateInfo.wireframeMode = false;
    rendererModule.initialize(window, rendererCreateInfo, &skyboxMat);
    
    messageBus.addReceiver( &rendererModule );

    objectModule.NewEntity(2);
    {
        auto mr = objectModule.NewComponent<MeshRenderer>();
            mr->material = &unlitColorMat;
            mr->mesh = &resourceModule.meshes.find("Resources/Models/House Dancing.fbx/Alpha_Surface")->second;

        auto t = objectModule.NewComponent<Transform>();
            t->getLocalPositionModifiable() = { 0.0f, 0.0f, -25.0f };
            t->setParent(&sceneModule.rootNode);
    }
    
    objectModule.NewEntity(2);
    {
        auto mr = objectModule.NewComponent<MeshRenderer>();
            mr->material = &unlitTextureMat;
            mr->mesh = &resourceModule.meshes.find("Resources/Models/House Dancing.fbx/Alpha_Joints")->second;

        auto t = objectModule.NewComponent<Transform>();
            t->getLocalPositionModifiable() = { 0.0f, 0.0f, -25.0f };
            t->setParent(&sceneModule.rootNode);
    }

    objectModule.NewEntity(2);
    {
        auto br = objectModule.NewComponent<BillboardRenderer>();
            br->material = &unlitInstancedMat;
        
        auto t = objectModule.NewComponent<Transform>();
            t->getLocalPositionModifiable() = { 10.0f, 0.0f, 0.0f };
            t->getLocalScaleModifiable() = { 10.0f, 10.0f, 10.0f };
    }

    objectModule.NewEntity(2);
    {
        auto br = objectModule.NewComponent<BillboardRenderer>();
            br->material = &unlitInstancedMat;
        
        auto t = objectModule.NewComponent<Transform>();
            t->getLocalPositionModifiable() = { 0.0f, 0.0f, 0.0f };
            t->getLocalScaleModifiable() = { 10.0f, 10.0f, 10.0f };
    }

    objectModule.NewEntity(2);
    {
        auto br = objectModule.NewComponent<BillboardRenderer>();
            br->material = &unlitInstancedMat;
        
        auto t = objectModule.NewComponent<Transform>();
            t->getLocalPositionModifiable() = { -10.0f, 0.0f, 0.0f };
            t->getLocalScaleModifiable() = { 10.0f, 10.0f, 10.0f };
    }

    objectModule.NewEntity(2);
    {
        auto br = objectModule.NewComponent<BillboardRenderer>();
            br->material = &unlitInstancedMat;
        
        auto t = objectModule.NewComponent<Transform>();
            t->getLocalPositionModifiable() = { 10.0f, 10.0f, 0.0f };
            t->getLocalScaleModifiable() = { 10.0f, 10.0f, 10.0f };
    }

    objectModule.NewEntity(2);
    {
        auto br = objectModule.NewComponent<BillboardRenderer>();
            br->material = &unlitInstancedMat;
        
        auto t = objectModule.NewComponent<Transform>();
            t->getLocalPositionModifiable() = { 0.0f, 10.0f, 0.0f };
            t->getLocalScaleModifiable() = { 10.0f, 10.0f, 10.0f };
    }

    objectModule.NewEntity(2);
    {
        auto br = objectModule.NewComponent<BillboardRenderer>();
            br->material = &unlitInstancedMat;
        
        auto t = objectModule.NewComponent<Transform>();
            t->getLocalPositionModifiable() = { -10.0f, 10.0f, 0.0f };
            t->getLocalScaleModifiable() = { 10.0f, 10.0f, 10.0f };
    }

    objectModule.NewEntity(4);
    {
        auto t = objectModule.NewComponent<Transform>();
            t->getLocalPositionModifiable() = { 0.5f, 0.0f, 10.0f };
            t->getLocalScaleModifiable() *= 10;
            t->setParent(&sceneModule.rootNode);

        auto c = objectModule.NewComponent<SphereCollider>();
            c->radius = 10;

        auto mr = objectModule.NewComponent<MeshRenderer>();
            mr->mesh = &resourceModule.meshes.find("Resources/Models/unit_sphere.fbx/Sphere001")->second;
            mr->material = &unlitTextureMat;

        so1 = objectModule.NewComponent<AudioSource>();
            so1->getClipsModifiable().push_back("Resources/Audios/test.wav");
            so1->getGainModifiable() = 8.0f;
            so1->getIsRelativeModifiable() = false;
            so1->getIsLoopingModifiable() = true;
    }

    objectModule.NewEntity(3);
    {
        auto t = objectModule.NewComponent<Transform>();
            t->getLocalPositionModifiable()={-0.5f,0.0f,10.0f};
            t->getLocalScaleModifiable() *= 10;
            t->setParent(&sceneModule.rootNode);

        auto c = objectModule.NewComponent<SphereCollider>();
            c->radius = 10;

        auto mr = objectModule.NewComponent<MeshRenderer>();
            mr->mesh = &resourceModule.meshes.find("Resources/Models/unit_sphere.fbx/Sphere001")->second;
            mr->material = &unlitColorMat;
    }

    gameSystemsModule.addSystem(&rendererSystem);
    gameSystemsModule.addSystem(&cameraControlSystem);
    gameSystemsModule.addSystem(&billboardSystem);
    gameSystemsModule.addSystem(&collisionDetectionSystem);

#pragma endregion

#pragma region Camera
    objectModule.NewEntity(4);
    {
        auto c = objectModule.NewComponent<Camera>();
            c->farPlane = 1000.0f;
            c->nearPlane = 0.01f;
            c->fieldOfView = 80.0f;
            c->projectionMode = CameraProjection::Perspective;

        
        auto t = objectModule.NewComponent<Transform>();
            t->getLocalPositionModifiable() = glm::vec3(0.0f, 0.0f, 0.0f);
            t->setParent(&sceneModule.rootNode);

        li = objectModule.NewComponent<AudioListener>();
            li->getIsCurrentModifiable() = true;
            li->getGainModifiable() = 1.0f;
            li->getVelocityModifiable();
            li->getPositionModifiable();
            li->getAtModifiable();
            li->getUpModifiable();

        auto sc = objectModule.NewComponent<SphereCollider>();
            sc->type = Collider::Type::KINEMATIC;
            sc->radius = 5;
    }
    CameraSystem::setAsMain(&objectModule.entities.back());

    gameSystemsModule.addSystem(&cameraSystem);

#pragma endregion

#pragma region AudioModule demo - initialization
    
    audioModule.init();


    so1->getListenersModifiable().push_back(li);
    /*
    objectModule.NewEntity(2);
    {
        so1 = objectModule.NewComponent<AudioSource>();
            so1->getListenersModifiable().push_back(li);
            so1->getClipsModifiable().push_back("Resources/Audios/test.wav");
            so1->getIsRelativeModifiable() = false;
            so1->getIsLoopingModifiable() = true;

        auto t = objectModule.NewComponent<Transform>();
            t->getLocalPositionModifiable() = { 0.0f, 0.0f, 0.0f };
            t->setParent(&sceneModule.rootNode);
    }
    */

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
    
    objectModule.NewEntity(2);
    {
        so3 = objectModule.NewComponent<AudioSource>();
            so3->getListenersModifiable().push_back(li);
            so3->getClipsModifiable().push_back("Resources/Audios/sample.wav");
            so3->getIsRelativeModifiable() = true;
            so3->getGainModifiable() = 0.25f;
            so3->getIsLoopingModifiable() = true;

        auto t = objectModule.NewComponent<Transform>();
            t->getLocalPositionModifiable() = { 0.0f, 0.0f, 0.0f };
            t->setParent(&sceneModule.rootNode);
    }

    gameSystemsModule.addSystem(&audioListenerSystem);
    gameSystemsModule.addSystem(&audioSourceSystem);
    
#pragma endregion

    gameSystemsModule.entities = &objectModule.entities;

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
            // Traverse the scene graph and update transforms
            sceneModule.updateTransforms();

            // ! ----- FIXED UPDATE FUNCTION -----
            
            gameSystemsModule.run(System::FIXED);

            // Decrease the lag by fixed step
            lag -= FIXED_TIME_STEP;
        }

        // ! ----- FRAME UPDATE FUNCTION -----

        gameSystemsModule.run(System::FRAME);
        // Read message bus before rendering
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
