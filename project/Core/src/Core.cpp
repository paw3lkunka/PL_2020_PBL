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


Core& GetCore()
{
    return *(Core::instance);
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
    float b = 0;
    float pulse = 0.01;
    //Main loop
    while (!glfwWindowShouldClose(window))
    {
		glClearColor(.2,.4,b,1);
		glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
        inputModule.captureControllersInput();

        messageBus.notify();
        gameSystemsModule.run();

        inputModule.clearFlags();

        //TODO sleep 16 minus frame render time
        #ifdef __linux__ 
            usleep(16);
        #elif _WIN32
            Sleep(16);
        #endif

        b += pulse;
        if ( b > 1 || b < 0)
        {
            pulse = -pulse;
        }
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
