#include "Core.hpp"
#include "imgui.h"
#include "examples/imgui_impl_opengl3.h"
#include "examples/imgui_impl_glfw.h"
#include "Components.inc"
#include "Systems.inc"

#include "Material.hpp"

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
    //glfwSwapInterval(0);

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
    if (recreateScene)
    {
        #include "../../resources/Scenes/scene_old.txt"
    }
    else
    {
        objectModule.readScene(sceneFilePath);
    }

    if (updateScene)
    {
        // ! Manual extension of scene, runned by -u param
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

    gameSystemsModule.addSystem(&rendererSystem);
    gameSystemsModule.addSystem(&cameraControlSystem);
    gameSystemsModule.addSystem(&collisionSystem);
    gameSystemsModule.addSystem(&physicalBasedInputSystem);
    gameSystemsModule.addSystem(&physicSystem);
    gameSystemsModule.addSystem(&skeletonSystem);
    gameSystemsModule.addSystem(&paddleControlSystem);

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

    gameSystemsModule.addSystem(&audioListenerSystem);
    gameSystemsModule.addSystem(&audioSourceSystem);
    
#pragma endregion

#pragma region Camera
    // ! Finding main camera
    CameraSystem::setAsMain(objectModule.getEntityPtrByName("Camera"));

    gameSystemsModule.addSystem(&cameraSystem);

#pragma endregion

    gameSystemsModule.entities = objectModule.getEntitiesVector();

    // ! IMGUI initialize
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    // ? Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430");
    // ? Setup Dear ImGui style
    ImGui::StyleColorsDark();


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
        //messageBus.sendMessage( Message(Event::AUDIO_SOURCE_PLAY, objectModule.getEntityPtrByName("sampleSound")->getComponentPtr<AudioSource>()) );
        //messageBus.sendMessage( Message(Event::AUDIO_SOURCE_PLAY, objectModule.getEntityPtrByName("sphereSound")->getComponentPtr<AudioSource>()));
#pragma endregion

    // * ===== Game loop ===================================================

    sceneModule.updateTransforms();
    // * pointer for entity
    Entity* e = objectModule.getEntityPtrByID(0u);
    Transform* eTrans = e->getComponentPtr<Transform>();

    int entitiesSize = (*objectModule.getEntitiesVector()).size();
    // * list of entities names with \0 in between
    std::string entities;
    for(int i = 0; i < entitiesSize; ++i)
    {
        entities += "ID ";
        entities += std::to_string(i);
        entities += " Name: ";
        entities += objectModule.getEntityPtrByID(i)->getName();
        entities += char(0);
    }
    // * index for combo list
    int currentItem = 0;
    // * rotation in eulers
    glm::vec3 worldRotation = glm::vec3(0);
    glm::vec3 localRotation = glm::vec3(0);
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
        
        // ? ++++ IMGUI NEW FRAME ++++
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ? +++++ FIXED UPDATE LOOP +++++

        while(lag >= FIXED_TIME_STEP)
        {
            // Read message bus messages
            messageBus.notify();

            // ! ----- FIXED UPDATE FUNCTION -----
            
            gameSystemsModule.run(System::FIXED);

            // Traverse the scene graph and update transforms
            sceneModule.updateTransforms();
            physicalBasedInputSystem.clearKeysets();

            // Decrease the lag by fixed step
            lag -= FIXED_TIME_STEP;
        }

        // ! ----- FRAME UPDATE FUNCTION -----

        gameSystemsModule.run(System::FRAME);
        // Read message bus before rendering
        // TODO: Should transform update be here also?
        messageBus.notify();

        // ? +++++ IMGUI WINDOW ++++
        ImGui::Begin("Edit window");
        if(ImGui::Combo("", &currentItem, entities.c_str()))
        {
            e = objectModule.getEntityPtrByID(currentItem);
            eTrans = e->getComponentPtr<Transform>();
            //rotation is quaternion in ZXY
            glm::quat worldRotDec = {1, 0, 0, 0};
            // I don't need this shit
            glm::vec3 shit3(1.0f);
            glm::vec4 shit(1.0f);
            glm::decompose(eTrans->localToWorldMatrix, shit3, worldRotDec, shit3, shit3, shit);
            glm::quat worldRot = worldRotDec * eTrans->getLocalRotation();
            worldRotation = glm::eulerAngles(worldRot) * 180.0f / glm::pi<float>();
            localRotation = glm::eulerAngles(eTrans->getLocalRotation()) * 180.0f / glm::pi<float>();
            //rotation = glm::vec3(tempRot.x, tempRot.z, tempRot.y);
        }

        ImGui::Text(("Entity: " + std::string(e->getName())).c_str());
        ImGui::Text("Transform (local):");
        ImGui::DragFloat3("Position: ", (float*)&eTrans->getLocalPositionModifiable(), 0.5f, -1000.0f, 1000.0f, "%.2f");
        if(ImGui::DragFloat3("Rotation: ", (float*)&localRotation, 0.5f, -360.0f, 360.0f, "%.1f"))
        {
            eTrans->getLocalRotationModifiable() = eulerToQuaternion(localRotation);
        }
        ImGui::DragFloat3("Scale: ", (float*)&eTrans->getLocalScaleModifiable(), 1.0f, 1.0f, 100.0f, "%.2f");
        ImGui::Text("Transform (World):");
        if(ImGui::DragFloat3("_Rotation: ", (float*)&worldRotation, 0.5f, -360.0f, 360.0f, "%.1f"))
        {
            glm::quat worldRotDec = {1, 0, 0, 0};
            // I don't need this shit
            glm::vec3 shit3(1.0f);
            glm::vec4 shit(1.0f);
            glm::decompose(eTrans->worldToLocalMatrix, shit3, worldRotDec, shit3, shit3, shit);
            glm::quat rot = worldRotDec * eulerToQuaternion(worldRotation);
            eTrans->getLocalRotationModifiable() = rot;
        }

        if(e->getComponentPtr<Paddle>() != nullptr)
        {
            ImGui::Text("Paddle: ");
            Paddle* paddle = e->getComponentPtr<Paddle>();
            ImGui::DragFloat3( "Max postition", (float*)&paddle->maxPos, 0.05f, -20.0f, 20.0f, "%.2f");
            ImGui::DragFloat("Min speed", (float*)&paddle->minSpeed, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Max speed", (float*)&paddle->maxSpeed, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Max front rotation ", (float*)&paddle->maxFrontRot, 0.5f, -90.0f, 90.0f);
            ImGui::DragFloat("Max side rotation ", (float*)&paddle->maxSideRot, 0.5f, -90.0f, 90.0f);
        }
        if(eTrans->getParent()->serializationID == 0)
        {
            ImGui::Text("Parent name: Root scene");
        }
        else
        {
            ImGui::Text(("Parent name: " + std::string(eTrans->getParent()->entityPtr->getName())).c_str());
        }
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
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
    //! IMGUI CLEANUP
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

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
