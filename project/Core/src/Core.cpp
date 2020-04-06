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

int Core::init()
{       
    std::cout << "Henlo!" << std::endl;
    //TODO: GLFW Error callback
    
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800, 600, "PBL", NULL, NULL);
    if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -2;
	}

    glViewport(0,0,800,600);
    
    //Initializing MessageBus

        messageBus = MessageBus(128);
        
        inputModule = InputModule( &messageBus );
        consoleModule = ConsoleModule( &messageBus );
        gameSystemsModule = GameSystemsModule(&messageBus);


    //Initializing Modules, and adding connecting to MB
    inputModule.initialize(window);

    messageBus.addReceiver( &inputModule );    
    messageBus.addReceiver( &consoleModule );
    messageBus.addReceiver( &gameSystemsModule );
    messageBus.addReceiver( &tmpExit );


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
}

void Core::cleanup()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}
