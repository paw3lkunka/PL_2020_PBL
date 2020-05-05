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

    FileSystemData unlitSkinnedVertexShaderData;
    unlitSkinnedVertexShaderData.path = "Resources/Shaders/UnlitSkinned/UnlitSkinned.vert";
    unlitSkinnedVertexShaderData.typeOfFile = FileType::SHADER;

    FileSystemData unlitSkinnedFragmentShaderData;
    unlitSkinnedFragmentShaderData.path = "Resources/Shaders/UnlitSkinned/UnlitSkinned.frag";
    unlitSkinnedFragmentShaderData.typeOfFile = FileType::SHADER;

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

    getMessageBus().sendMessage(Message(Event::LOAD_FILE, unlitColorVertexShaderData));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, unlitColorFragmentShaderData));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, unlitTextureVertexShaderData));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, unlitTextureFragmentShaderData));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, unlitInstancedVertexShaderData));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, unlitInstancedFragmentShaderData));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, unlitSkinnedVertexShaderData));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, unlitSkinnedFragmentShaderData));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, skyboxCubemapVertexShaderData));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, skyboxCubemapFragmentShaderData));
    //getMessageBus().sendMessage(Message(Event::LOAD_FILE, testModel));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, animModel));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, testTexture));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, skyNX));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, skyNY));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, skyNZ));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, skyPX));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, skyPY));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, skyPZ));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, fsData));
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
    skinnedShader = Shader( resourceModule.shaders.find("Resources/Shaders/UnlitSkinned/UnlitSkinned.vert")->second.c_str(),
                            resourceModule.shaders.find("Resources/Shaders/UnlitSkinned/UnlitSkinned.frag")->second.c_str());

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

    skinnedMat = Material(&skinnedShader);

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

    skinnedMat.setVec4("color", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
    // auto e1 = objectModule.FindEntity("Alpha_Surface");
    // e1->getComponentPtr<MeshRenderer>()->material = &unlitTextureMat;

    // auto e2 = objectModule.FindEntity("Plane_skin");
    // e2->getComponentPtr<MeshRenderer>()->material = &skinnedMat;

    auto e1 = objectModule.FindEntity("Alpha_Surface");
    e1->getComponentPtr<MeshRenderer>()->material = &skinnedMat;

    auto e2 = objectModule.FindEntity("Alpha_Joints");
    e2->getComponentPtr<MeshRenderer>()->material = &skinnedMat;

    // auto e3 = objectModule.FindEntity("Sphere");
    // e3->getComponentPtr<MeshRenderer>()->material = &unlitColorMat;
    
    // auto e4 = objectModule.FindEntity("Box");
    // e4->getComponentPtr<MeshRenderer>()->material = &unlitColorMat;

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

    gameSystemsModule.addSystem(&rendererSystem);
    gameSystemsModule.addSystem(&cameraControlSystem);
    gameSystemsModule.addSystem(&billboardSystem);
    gameSystemsModule.addSystem(&skeletonSystem);

#pragma endregion

#pragma region Camera
    objectModule.NewEntity(2);
    {
        auto c = objectModule.NewComponent<Camera>();
            c->farPlane = 10000.0f;
            c->nearPlane = 0.01f;
            c->fieldOfView = 80.0f;
            c->projectionMode = CameraProjection::Perspective;

        
        auto t = objectModule.NewComponent<Transform>();
            t->getLocalPositionModifiable() = glm::vec3(0.0f, 0.0f, 0.0f);
            t->setParent(&sceneModule.rootNode);
    }
    CameraSystem::setAsMain(&objectModule.entities.back());

    gameSystemsModule.addSystem(&cameraSystem);

#pragma endregion

#pragma region AudioModule demo - initialization
    
    audioModule.init();
    
    objectModule.NewEntity(2);
    {
        li = objectModule.NewComponent<AudioListener>();
            li->getIsCurrentModifiable() = true;

        auto t = objectModule.NewComponent<Transform>();
            t->getLocalPositionModifiable() = { 0.0f, 0.0f, 0.0f };
            t->setParent(&sceneModule.rootNode);
    }

    objectModule.NewEntity(2);
    {
        so = objectModule.NewComponent<AudioSource>();
            so->getListenersModifiable().push_back(li);
            so->getClipsModifiable().push_back("Resources/Audios/test.wav");
            so->getIsRelativeModifiable() = true;
            so->getIsLoopingModifiable() = true;

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
        messageBus.sendMessage( Message(Event::AUDIO_SOURCE_PLAY, so) );
#pragma endregion

    // * ===== Game loop ===================================================

    //Main loop
    while (!glfwWindowShouldClose(window))
    {
#pragma region AudioModule demo
        auto tran = li->entityPtr->getComponentPtr<Transform>();
        tran->getLocalPositionModifiable() = tran->getLocalPosition() - glm::vec3(2.0f, 0.0f, 0.0f);
        if(tran->getLocalPosition().x < -50.0f)
        {
            tran->getLocalPositionModifiable() = glm::vec3(50.0f, 0.0f, 0.0f);
        }
#pragma endregion

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
