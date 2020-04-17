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

// ! For material testing only
#include "Material.hpp"
#include "FileStructures.inl"

Core& GetCore()
{
    return *(Core::instance);
}

void InfoLog(const char* log) 
{ 
    Core::instance->messageBus.sendMessage(Message(Event::DEBUG_INFO_LOG, log));
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

    window = glfwCreateWindow(INIT_WINDOW_WIDTH, INIT_WINDOW_HIGHT, "PBL", NULL, NULL);
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

    glViewport(0,0,INIT_WINDOW_WIDTH,INIT_WINDOW_HIGHT);
    
    //Initializing Modules, and adding connecting to MB
    inputModule.initialize(window);

    messageBus.addReceiver( &inputModule );    
    messageBus.addReceiver( &consoleModule );
    messageBus.addReceiver( &gameSystemsModule );
    messageBus.addReceiver( &resourceModule );
    messageBus.addReceiver( &tmpExit );

#pragma region mock Material

    FileSystemData fsData;
    fsData.path = "Resources/Shaders/UnlitColor/UnlitColor.frag";
    fsData.typeOfFile = FileType::SHADER;

    GetCore().getMessageBus().sendMessage(Message(Event::LOAD_FILE, fsData));
    GetCore().getMessageBus().notify();
    GetCore().getMessageBus().sendMessage(Message(Event::QUERY_SHADER_DATA, "Resources/Shaders/UnlitColor/UnlitColor.frag"));
    GetCore().getMessageBus().notify();

    //std::string vertexShader = readTextFile("Resources/Shaders/UnlitColor/UnlitColor.vert");
    //std::string fragmentShader = readTextFile("Resources/Shaders/UnlitColor/UnlitColor.frag");

    //Shader testShader(vertexShader.c_str(), fragmentShader.c_str());
    //Material testMaterial(&testShader);

    //testMaterial.use();

#pragma endregion

#pragma region mock ECS
    e1.addComponent(&t1);
    e1.addComponent(&mockComponent);
    
    e2.addComponent(&t2);
    e2.addComponent(&otherComponent);
    e2.addComponent(&mockComponent2);

    e3.addComponent(&t3);
    //nie powinniśmy współdzielić komponentu między dwoma encjami, ale ćiiiii..., to tylko mock xD
    e3.addComponent(&otherComponent);

    sceneModule.rootNode.children.push_back(&t1);
    sceneModule.rootNode.children.push_back(&t2);
    t2.children.push_back(&t3);

    t1.parent = &(sceneModule.rootNode);
    t2.parent = &(sceneModule.rootNode);
    t3.parent = &t2;

    t3.localPosition = {1,0,0};
    
    gameSystemsModule.addEntity(&e1);
    gameSystemsModule.addEntity(&e2);
    gameSystemsModule.addEntity(&e3);

    gameSystemsModule.addSystem(&mockSystem);
    gameSystemsModule.addSystem(&mockReporter);
#pragma endregion

    

    GetCore().getMessageBus().notify();
    // Everything is ok.
    return 0;
}

int Core::mainLoop()
{
    //TODO
#pragma region for MOCK screen animation
    float b = 0;
    float pulse = 0.01;
#pragma endregion

    double previousFrameStart = glfwGetTime();
    double lag = 0;

    //Main loop
    while (!glfwWindowShouldClose(window))
    {
        double currentFrameStart = glfwGetTime();
        double lastFrameTime = currentFrameStart - previousFrameStart;

        lag += lastFrameTime;
        previousFrameStart = currentFrameStart;
                
        glfwSwapBuffers(window);
        glfwPollEvents();
        inputModule.captureControllersInput();
        
        while(lag >= FIXED_TIME_STEP)
        {
            InfoLog("UPDATE");
            messageBus.notify();
#pragma region MOCK scene update
            mockReporter.counter = 1;
            t1.localPosition += glm::vec3(0.1,0.1,0.1);
            t1.dirty = true;
            t2.localRotation = glm::quat({0,glm::pi<double>()/2.0,0}) * t2.localRotation;
            t2.dirty = true;
#pragma endregion
            sceneModule.updateTransforms();

            gameSystemsModule.run();
            lag -= FIXED_TIME_STEP;
        }

        inputModule.clearFlags();

        //TODO RENDER FRAME
#pragma region MOCK screen animation
		glClearColor(.2,.4,b,1);
		glClear(GL_COLOR_BUFFER_BIT);
        b += pulse;
        if ( b > 1 || b < 0)
        {
            pulse = -pulse;
        }
#pragma endregion

        InfoLog("FRAME");
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
