// standard C++ library
#include <iostream>
#include <utility>

// system-depended
#ifdef __linux__ 
    #include <unistd.h>
#elif _WIN32
    #include <windows.h>
#endif

// external libraries
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// our code
#include "InputModule.hpp"
#include "ConsoleModule.hpp"
#include "MessageBus.hpp"
#include "Message.inl"
#include "GameSystemsModule.hpp"
#include "Entity.hpp"
#include "Component.inl"
#include "System.hpp"

typedef std::pair<float,float> float2;

int main()
{
    std::cout << "Henlo!" << std::endl;

    Entity e1(0, 5), e2(1, 5), e3(3, 5);

    //TODO: tak w ogóle to to będzie klasa Core


    //TODO: GLFW Error callback
    
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "PBL", NULL, NULL);
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
		return -1;
	}

    glViewport(0,0,800,600);
    
	glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset){exit(0);});

    class MockComponent : public Component
    {
        public:
        int a; int b;
        MockComponent(int x, int y) { a = x; b = y;}
    } mockComponent(2, 2), mockComponent2(21, 37);

    class MockInvalidComponent : public Component
    {
        bool aaa;
    } otherComponent;

    e1.addComponent(&mockComponent);
    
    e2.addComponent(&otherComponent);
    e2.addComponent(&mockComponent2);

    e3.addComponent(&otherComponent);

    struct MockSystem : public System
    {
        MockComponent* ptr;
        virtual bool assertEntity(Entity* entity)
        {
            ptr = entity->getComponent<MockComponent>();
            return ptr != nullptr;
        }

        virtual void update()
        {
            int a = ptr->a, b = ptr->b;
            //std::cout << a << "+" << b << "=" << a + b << std::endl;
        }
    } mockSystem;

    //Some early test
    Message m1(Event::KEY_PRESSED);
    Message m2(Event::KEY_PRESSED, 123);
    Message m3(Event::KEY_PRESSED, 0.2);
    Message m4(Event::KEY_PRESSED, float2(21.0f, 37.0f));
    std::cout << m2.getValue<int>() << ", " << m3.getValue<double>() << std::endl;
    std::cout << m4.getValue<float2>().first << ", " << m4.getValue<float2>().second << std::endl;

    //Initializing MessageBus
    MessageBus messageBus(128);

    //Initializing Modules, and adding connecting to MB
    InputModule inputModule( &messageBus );
    inputModule.initialize(window);
    messageBus.addReceiver( &inputModule );

    ConsoleModule consoleModule( &messageBus );
    messageBus.addReceiver( &consoleModule );
    
    GameSystemsModule gameSystemsModule(&messageBus);
    gameSystemsModule.addEntity(&e1);
    gameSystemsModule.addEntity(&e2);
    gameSystemsModule.addEntity(&e3);
    gameSystemsModule.addSystem(&mockSystem);
    messageBus.addReceiver( &gameSystemsModule );

    //Anonymous class module
    class : public IModule
    {
        virtual void receiveMessage(Message msg)
        {
            if(msg.getEvent() == Event::KEY_PRESSED && msg.getValue<int>() == GLFW_KEY_ESCAPE)
            {
                exit(0);
            }
        }
    } tmpExit;
        
    messageBus.addReceiver( &tmpExit );



    //Main loop
    while (!glfwWindowShouldClose(window))
    {
		glClearColor(.2,.4,1,1);
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
    }    

    
	glfwDestroyWindow(window);
	glfwTerminate();
    return 0;
}