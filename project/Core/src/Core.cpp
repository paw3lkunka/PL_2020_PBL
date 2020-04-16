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
    messageBus.addReceiver( &tmpExit );

#pragma region mock ECS
    e1.addComponent(&mockComponent);
    
    e2.addComponent(&otherComponent);
    e2.addComponent(&mockComponent2);
    //nie powinniśmy współdzielić komponentu między dwoma encjami, ale ćiiiii..., to tylko mock xD
    e3.addComponent(&otherComponent);
    
    gameSystemsModule.addEntity(&e1);
    gameSystemsModule.addEntity(&e2);
    gameSystemsModule.addEntity(&e3);
    gameSystemsModule.addSystem(&mockSystem);
#pragma endregion

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
