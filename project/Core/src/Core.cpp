#include "Core.hpp"
#include "imgui.h"

#include "Components.inc"
#include "Systems.inc"
#include "MomentOfInertia.hpp"

#include "Material.hpp"
#include "ScenesPaths.inl"
#include "ModelsPaths.inl"

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

GLFWwindow* Core::getWindowPtr()
{
    return window;
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
    glfwSwapInterval(0);

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
    messageBus.addReceiver( &uiModule );
    messageBus.addReceiver( &tmpExit );

    // ! Scene loading
    if (recreateScene)
    {
        // ? -r
        #include "../../resources/Scenes/selectCargoScene.icpp"
        //#include "../../resources/Scenes/main_Menu.icpp"
        //#include "../../resources/Scenes/scene_old.icpp"
        //#include "../../resources/Scenes/testScene.icpp"
    }
    else
    {
        // ? none, or -f
        objectModule.readScene(sceneFilePath);
    }

    if (updateScene)
    {
        // ! Manual extension of scene
        // ? -u
        {
            
        }

        objectModule.saveScene("../resources/Scenes/savedScene.json");
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
    rendererModule.initialize(window, rendererCreateInfo, objectModule.getMaterialPtrByName("skyboxMat"));
    
    messageBus.addReceiver( &rendererModule );
#pragma endregion

    // ! IK system initialize
    BoneAttachData leftData;
    leftData.attachEntityPtr = objectModule.getEntityPtrByName("Paddle_attach_left");
    leftData.boneEntity = objectModule.getEntityPtrByName("kajak_wjoslo_plastus.FBX/End_left");

    BoneAttachData rightData;
    rightData.attachEntityPtr = objectModule.getEntityPtrByName("Paddle_attach_right");
    rightData.boneEntity = objectModule.getEntityPtrByName("kajak_wjoslo_plastus.FBX/End_right");

    Entity* skelly = objectModule.getEntityPtrByName("Spine_skeleton");
    //paddleIkSystem.init(leftData, rightData, skelly->getComponentPtr<Skeleton>());
    //gameSystemsModule.addSystem(&paddleIkSystem);

#pragma region AudioModule demo - initialization

    audioModule.init();

#pragma endregion

#pragma region Camera
    // ! Finding main camera
    CameraSystem::setAsMain(objectModule.getEntityPtrByName("Camera"));

#pragma endregion

    gameSystemsModule.entities = objectModule.getEntitiesVector();

    uiModule.init();

    // ! IMGUI initialize
    editorModule.init(window);

#pragma regnon attach systems

    gameSystemsModule.addSystem(&hydroBodySystem);
    gameSystemsModule.addSystem(&hideoutSystem);
    gameSystemsModule.addSystem(&rendererSystem);
    gameSystemsModule.addSystem(&cameraControlSystem);
    gameSystemsModule.addSystem(&collisionSystem);
    gameSystemsModule.addSystem(&physicalBasedInputSystem);
    gameSystemsModule.addSystem(&physicSystem);
    gameSystemsModule.addSystem(&skeletonSystem);
    gameSystemsModule.addSystem(&paddleControlSystem);
    gameSystemsModule.addSystem(&audioListenerSystem);
    gameSystemsModule.addSystem(&audioSourceSystem);
    gameSystemsModule.addSystem(&lightSystem);
    gameSystemsModule.addSystem(&cameraSystem);
    gameSystemsModule.addSystem(&uiRendererSystem);
    gameSystemsModule.addSystem(&uiButtonSystem);
    gameSystemsModule.addSystem(&enemySystem);
    gameSystemsModule.addSystem(&sortingGroupSystem);
    gameSystemsModule.addSystem(&toggleButtonSystem);
    gameSystemsModule.addSystem(&cargoStorageSystem);

#pragma endregion

    // Everything is ok.
    return 0;
}

int Core::mainLoop()
{
    double previousFrameStart = glfwGetTime();
    //HACK temporary solution, should be 0 n start
    double lag = FIXED_TIME_STEP;

#pragma region AudioModule demo
        // messageBus.sendMessage( Message(Event::AUDIO_SOURCE_PLAY, objectModule.getEntityPtrByName("sampleSound")->getComponentPtr<AudioSource>()) );
        // messageBus.sendMessage( Message(Event::AUDIO_SOURCE_PLAY, objectModule.getEntityPtrByName("sphereSound")->getComponentPtr<AudioSource>()));
#pragma endregion

    // * ===== Game loop ===================================================

    sceneModule.updateTransforms();
    uiModule.updateRectTransforms();
    editorModule.setup();

    // ! ----- START SYSTEM FUNCTION -----

    gameSystemsModule.run(System::START);
    //Main loop
    while (!glfwWindowShouldClose(window))
    {

        // ? +++++ FIXED UPDATE TIME MANAGEMENT +++++

            currentFrameStart = glfwGetTime();
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
            uiModule.updateRectTransforms();

            hideoutSystem.clean();
            physicalBasedInputSystem.clearKeysets();

            // Decrease the lag by fixed step
            lag -= FIXED_TIME_STEP;
        }

        // ! ----- FRAME UPDATE FUNCTION -----

        gameSystemsModule.run(System::FRAME);
        // Read message bus before rendering
        // TODO: Should transform update be here also?
        messageBus.notify();

        // ? IMGUI Window setting up
        editorModule.drawEditor();

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

double Core::getCurrentFrameStart()
{
    return currentFrameStart;
}

MessageBus& Core::getMessageBus()
{
    return messageBus;
}

void Core::cleanup()
{
    editorModule.onExit();

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
CollisionSystem Core::collisionSystem;
PhysicalBasedInputSystem Core::physicalBasedInputSystem;
PhysicSystem Core::physicSystem;
SkeletonSystem Core::skeletonSystem;
PaddleControlSystem Core::paddleControlSystem;
PaddleIkSystem Core::paddleIkSystem;
LightSystem Core::lightSystem;
UiRendererSystem Core::uiRendererSystem;
HydroBodySystem Core::hydroBodySystem;
UiButtonSystem Core::uiButtonSystem;
HideoutSystem Core::hideoutSystem;
EnemySystem Core::enemySystem;
SortingGroupSystem Core::sortingGroupSystem;
ToggleButtonSystem Core::toggleButtonSystem;
CargoStorageSystem Core::cargoStorageSystem;