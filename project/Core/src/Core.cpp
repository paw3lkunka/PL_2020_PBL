#include "Core.hpp"

// standard C++ library
#include <iostream>
#include <utility>

// system-depended
#ifdef __linux__ 
    #include <unistd.h>
#elif _WIN32
    #include <windows.h>
#endif
Core* Core::instance = nullptr;

#include <sstream>

#include "RendererSystem.hpp"
#include "FileStructures.inl"

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

    window = glfwCreateWindow(INIT_WINDOW_WIDTH, INIT_WINDOW_HEIGHT, "PBL", NULL, NULL);
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

    glViewport(0,0,INIT_WINDOW_WIDTH,INIT_WINDOW_HEIGHT);
    
    //Initializing Modules, and adding connecting to MB
    inputModule.initialize(window);

    // ! ----- Renderer initialization block -----
    RendererModuleCreateInfo rendererCreateInfo = {};
    rendererCreateInfo.clearColor = glm::vec3(0.0f, 1.0f, 0.0f);
    rendererCreateInfo.clearFlags = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
    rendererCreateInfo.cullFace = true;
    rendererCreateInfo.cullFaceMode = GL_BACK;
    rendererCreateInfo.cullFrontFace = GL_CCW;
    rendererCreateInfo.depthTest = true;
    rendererCreateInfo.wireframeMode = false;
    rendererModule.initialize(window, rendererCreateInfo);

    messageBus.addReceiver( &inputModule );    
    messageBus.addReceiver( &consoleModule );
    messageBus.addReceiver( &gameSystemsModule );
    messageBus.addReceiver( &resourceModule );
    messageBus.addReceiver( &rendererModule );
    messageBus.addReceiver( &tmpExit );

#pragma region Data Loading

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

    FileSystemData testModel;
    testModel.path = "Resources/Models/Test.FBX";
    testModel.typeOfFile = FileType::MESH;

    FileSystemData animModel;
    animModel.path = "Resources/Models/House Dancing.FBX";
    animModel.typeOfFile = FileType::MESH;

    FileSystemData testTexture;
    testTexture.path = "Resources/Textures/tex.png";
    testTexture.typeOfFile = FileType::TEXTURE;

    getMessageBus().sendMessage(Message(Event::LOAD_FILE, unlitColorVertexShaderData));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, unlitColorFragmentShaderData));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, unlitTextureVertexShaderData));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, unlitTextureFragmentShaderData));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, testModel));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, animModel));
    getMessageBus().sendMessage(Message(Event::LOAD_FILE, testTexture));
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
    unlitColorMat = Material(&unlitColor);
    unlitColorMat.setVec4("color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

    TextureData texData = resourceModule.textures.find("Resources/Textures/tex.png")->second;

    TextureCreateInfo texCreateInfo = {};
    texCreateInfo.format = GL_RGBA;
    texCreateInfo.generateMipmaps = true;
    texCreateInfo.magFilter = GL_LINEAR;
    texCreateInfo.minFilter = GL_LINEAR_MIPMAP_LINEAR;
    texCreateInfo.width = texData.width;
    texCreateInfo.height = texData.height;
    Texture texture(texData.data, texCreateInfo);

    unlitTextureMat = Material(&unlitTexture);
    unlitTextureMat.setTexture("mainTex", texture);

    mr0.material = &unlitColorMat;
    mr0.mesh = &resourceModule.meshes.find("Resources/Models/House Dancing.FBX/Alpha_Surface")->second;

    mr1.material = &unlitTextureMat;
    mr1.mesh = &resourceModule.meshes.find("Resources/Models/House Dancing.FBX/Alpha_Joints")->second;

    testEntity0.addComponent(&mr0);
    testEntity1.addComponent(&mr1);

    t0.getLocalPositionModifiable() = { 0.0f, 0.0f, 0.0f };
    t1.getLocalPositionModifiable() = { 0.0f, 0.0f, 0.0f };

    testEntity0.addComponent(&t0);
    testEntity1.addComponent(&t1);
    sceneModule.rootNode.children.push_back(&t0);
    sceneModule.rootNode.children.push_back(&t1);

    gameSystemsModule.addEntity(&testEntity0);
    gameSystemsModule.addEntity(&testEntity1);

    gameSystemsModule.addSystem(&rendererSystem);
    gameSystemsModule.addSystem(&cameraControlSystem);

#pragma endregion

#pragma region Camera

    mainCamera.isMain = true;
    mainCamera.farPlane = 1000.0f;
    mainCamera.nearPlane = 0.01f;
    mainCamera.fieldOfView = 80.0f;
    mainCamera.projectionMode = CameraProjection::Perspective;

    cameraTransform.getLocalPositionModifiable() = glm::vec3(1.0f, 0.0f, 50.0f);

    cameraEntity.addComponent(&mainCamera);
    cameraEntity.addComponent(&cameraTransform);
    sceneModule.rootNode.children.push_back(&cameraTransform);

    gameSystemsModule.addEntity(&cameraEntity);
    gameSystemsModule.addSystem(&cameraSystem);

#pragma endregion

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

    // * ===== Game loop ===================================================

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
            // Travese the scene grpah and update transforms
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


MessageBus& Core::getMessageBus()
{
    return messageBus;
}

void Core::cleanup()
{
	glfwDestroyWindow(window);
	glfwTerminate();
    
    std::cout << "App freed resources properly." << std::endl;
}

void Core::close()
{
    glfwSetWindowShouldClose(window,true);
}
