#include "Core.hpp"
#include "Utils.hpp"
#include "imgui.h"

#include "xoshiro.h"

#include "Components.inc"
#include "Systems.inc"
#include "MomentOfInertia.hpp"

#include "Material.hpp"
#include "CubemapHdr.hpp"
#include "ScenesPaths.hpp"
#include "ModelsPaths.inl"
#include "TerrainUtils.hpp"

#include "glm/gtx/string_cast.hpp"

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
    xoshiro_Init();
    physicModule.init();

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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    if (debugOpengl)
    {
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    }

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

    if (debugOpengl)
    {
        int flags; 
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
        {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
            glDebugMessageCallback(glDebugOutput, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        }
    }

    //Initializing Modules, and adding connecting to MB
    inputModule.initialize(window);

    messageBus.addReceiver( &inputModule ); 
    messageBus.addReceiver( &consoleModule );
    messageBus.addReceiver( &gameSystemsModule );
    messageBus.addReceiver( &audioModule );
    messageBus.addReceiver( &objectModule );
    messageBus.addReceiver( &uiModule );
    messageBus.addReceiver( &gamePlayModule );

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
    rendererModule.initialize(window, rendererCreateInfo);

    messageBus.addReceiver( &rendererModule );
#pragma endregion

    // ! Scene loading
    if (recreateScene)
    {
        // ? -r
        //#include "../../resources/Scenes/main_Menu.icpp"
        //#include "../../resources/Scenes/selectCargoScene.icpp"
        //#include "../../resources/Scenes/scene_old.icpp"
        #include "../../resources/Scenes/newScene.icpp"
        //#include "../../resources/Scenes/intro.icpp"
    }
    else if (testScene)
    {
        #include "../../resources/Scenes/testScene.icpp"
    }
    else
    {
        // ? none, or -f
        {
            //various shit.
        }
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

    // TODO <make this function>
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
    // TODO </make this function>

    // ! Finding main camera
    CameraSystem::setAsMain(objectModule.getEntityPtrByName("Camera"));

    gameSystemsModule.entities = objectModule.getEntitiesVector();

    gamePlayModule.initScreens();

    uiModule.init();

    // ! IMGUI initialize
    editorModule.init(window);

#pragma regnon attach systems

    gameSystemsModule.addSystem(&hideoutSystem);
    gameSystemsModule.addSystem(&rendererSystem);

    gameSystemsModule.addSystem(&freeCameraControlSystem);
    gameSystemsModule.addSystem(&firstPersonCameraControlSystem);
    gameSystemsModule.addSystem(&thirdPersonCameraControlSystem);

    gameSystemsModule.addSystem(&terrainSystem);
    gameSystemsModule.addSystem(&physicalBasedInputSystem);

    gameSystemsModule.addSystem(&hydroBodySystem);
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
    gameSystemsModule.addSystem(&cargoSystem);
    gameSystemsModule.addSystem(&cargoStorageSystem);
    gameSystemsModule.addSystem(&cargoButtonSystem);
    gameSystemsModule.addSystem(&detectionBarSystem);
    gameSystemsModule.addSystem(&progressBarSystem);

#pragma endregion

    audioModule.init();
    cargoButtonSystem.init();

    // Everything is ok.
    return 0;
}

int Core::mainLoop()
{
    double previousFrameStart = glfwGetTime();
    //HACK temporary solution, should be 0 n start
    double lag = FIXED_TIME_STEP;

    // * ===== Game loop ===================================================

    sceneModule.updateTransforms();
    uiModule.updateRectTransforms();
    editorModule.setup();
    detectionBarSystem.init("DetectionProgressBar");
    gamePlayModule.init("Health_Bar", 30.0f);

    // ! ----- START SYSTEM FUNCTION -----

    // gameSystemsModule.run(System::START);
    // messageBus.notify();
    // audioModule.sceneInit();
    // TODO: Is this working every time? It should indeed
    sceneInit();
    
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
            if (!gamePaused)
            {
                physicModule.physicSimulation(gameSystemsModule.entities);
            }
            // Traverse the scene graph and update transforms
            sceneModule.updateTransforms();
            uiModule.updateRectTransforms();

            hideoutSystem.clean();
            physicalBasedInputSystem.clearKeysets();

            // Decrease the lag by fixed step
            lag -= FIXED_TIME_STEP;
        }

        // ! ----- FRAME UPDATE FUNCTION -----
        detectionBarSystem.prepare();
        gameSystemsModule.run(System::FRAME);
        // Read message bus before rendering
        // TODO: Should transform update be here also?
        messageBus.notify();
        // ? IMGUI Window setting up
        editorModule.drawEditor();
        //HACK i added this here, tu apply changes to model matrix;
        sceneModule.updateTransforms();
        // ? +++++ RENDER CURRENT FRAME +++++
        rendererModule.render();
        // Clear input flags at the end of frame
        inputModule.clearFlags();
    }    

    return 0;
}

void Core::sceneUnload()
{
    inputModule.clearFlags();
    physicModule.cleanup();
    
    audioModule.sceneUnload();
    // ! removing all associations for scene root node
    sceneModule.unloadScene();
    // ! removing all root nodes from UI
    uiModule.unloadScene();
    rendererModule.cleanAllPointers();
    CameraSystem::mainCamera = nullptr;
}

void Core::sceneInit()
{
    physicModule.init();
    // ! Finding main camera
    CameraSystem::setAsMain(objectModule.getEntityPtrByName("Camera"));
    
    // ! some setup
    sceneModule.updateTransforms();
    uiModule.updateRectTransforms();
    editorModule.setup();

    // ! cargo storage system initialize - must be before system start and notify!
    auto e = objectModule.getEntityPtrByName("AllWeightText");
    if (e != nullptr)
    {
        cargoStorageSystem.initTexts
        (
            e->getComponentPtr<TextRenderer>(), 
            objectModule.getEntityPtrByName("AllIncomeText")->getComponentPtr<TextRenderer>(),
            objectModule.getEntityPtrByName("group1")->getComponentPtr<UiSortingGroup>()
        );
    }
    else
    {
        cargoStorageSystem.initTexts(nullptr, nullptr, nullptr);
    }

    // ! ----- START SYSTEM FUNCTION -----
    gameSystemsModule.run(System::START);
    messageBus.notify();

    audioModule.sceneInit();
    
    detectionBarSystem.init("DetectionProgressBar");
    gamePlayModule.init("Health_Bar", 30.0f);
    
}

void Core::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    windowWidth = width;
    windowHeight = height;
    GetCore().getMessageBus().sendMessage(Message(Event::WINDOW_RESIZED, glm::ivec2(width, height)));
    GetCore().getMessageBus().notify();
}

void APIENTRY Core::glDebugOutput(GLenum source, 
                            GLenum type, 
                            GLuint id, 
                            GLenum severity, 
                            GLsizei length, 
                            const GLchar *message, 
                            const GLvoid *userParam)
{
    // ignore non-significant error/warning codes
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " <<  message << std::endl;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break; 
        case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;
    
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
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

    physicModule.cleanup();
    objectModule.cleanup();
    audioModule.cleanup();

	glfwDestroyWindow(window);
	glfwTerminate();
    
    std::cout << "App freed resources properly." << std::endl;
}

void Core::close()
{
    glfwSetWindowShouldClose(window,true);
}

int Core::randomInt()
{
    return xoshiro_next();
}


int Core::randomInt(int max)
{
    return xoshiro_next() % (max + 1);
}

int Core::randomInt(int min, int max)
{
    return min + (xoshiro_next() % (max + 1 - min));
}

float Core::randomFloat01R()
{
    union
    {
        uint32_t i;
        float f;
    } number;

    number.i = xoshiro_next();
    number.i = 0x3F800000U | (number.i >> 9);

    return number.f - 1.0f;
}
float Core::randomFloat01L()
{
    union
    {
        uint32_t i;
        float f;
    } number;

    number.i = xoshiro_next();
    number.i = 0x3F800000U | (number.i >> 9);

    return 2.0f - number.f;
}

float Core::randomFloatL(float max)
{
    return std::lerp(0, max, randomFloat01L());
}

float Core::randomFloatR(float max)
{
    return std::lerp(0, max, randomFloat01R());
}

float Core::randomFloatL(float min, float max)
{
    return std::lerp(min, max, randomFloat01L());
}

float Core::randomFloatR(float min, float max)
{
    return std::lerp(min, max, randomFloat01R());
}

CameraSystem Core::cameraSystem;
FreeCameraControlSystem Core::freeCameraControlSystem;
FirstPersonCameraControlSystem Core::firstPersonCameraControlSystem;
ThirdPersonCameraControlSystem Core::thirdPersonCameraControlSystem;
AudioSourceSystem Core::audioSourceSystem;
AudioListenerSystem Core::audioListenerSystem;
MeshRendererSystem Core::rendererSystem;
TerrainRendererSystem Core::terrainSystem;
PhysicalBasedInputSystem Core::physicalBasedInputSystem;
PhysicSystem Core::physicSystem;
SkeletonSystem Core::skeletonSystem;
PaddleControlSystem Core::paddleControlSystem;
PaddleIkSystem Core::paddleIkSystem;
LightSystem Core::lightSystem;
UiRendererSystem Core::uiRendererSystem;
UiButtonSystem Core::uiButtonSystem;
HideoutSystem Core::hideoutSystem;
EnemySystem Core::enemySystem;
HydroBodySystem Core::hydroBodySystem;
SortingGroupSystem Core::sortingGroupSystem;
ToggleButtonSystem Core::toggleButtonSystem;
CargoStorageSystem Core::cargoStorageSystem;
CargoButtonSystem Core::cargoButtonSystem;
DetectionBarSystem Core::detectionBarSystem;
ProgressBarSystem Core::progressBarSystem;
CargoSystem Core::cargoSystem;