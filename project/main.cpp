#include "InputModule.hpp"
#include "ConsoleModule.hpp"
#include "MessageBus.hpp"
#include <iostream>
#include <utility>

#ifdef __linux__ 
    #include <unistd.h>
#elif _WIN32
    #include <windows.h>
#endif

#include "GameSystemsModule.hpp"
#include "Entity.hpp"
#include "Component.inl"
#include "System.hpp"

typedef std::pair<float,float> float2;

int main()
{
    std::cout << "Henlo!" << std::endl;

    Entity e1(0,5), e2(1,5), e3(3,5);

    class MockComponent : public Component
    {
        public:
        int a; int b;
        MockComponent(int x, int y) { a=x; b=y;}
    } mockComponent(2,2), mockComponent2(21,37);

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
        virtual bool AssertEntity(Entity* entity)
        {
            ptr = entity->getComponent<MockComponent>();
            return ptr != nullptr;
        }

        virtual void Update()
        {
            int a = ptr->a, b = ptr->b;
            std::cout << a << "+" << b << "=" << a+b << std::endl;
        }
    } mockSystem;


    //Some early test
    Message m1(Event::KEY_PRESSED_A);
    Message m2(Event::KEY_PRESSED_A, 123);
    Message m3(Event::KEY_PRESSED_A, 0.2);
    Message m4(Event::KEY_PRESSED_A, float2(21.0f,37.0f));
    std::cout << m2.getValue<int>() << ", " << m3.getValue<double>() << std::endl;
    std::cout << m4.getValue<float2>().first << ", " << m4.getValue<float2>().second << std::endl;

    //Initializing MessageBus
    MessageBus messageBus(128);

    //Initializing Modules, and adding connecting to MB
    InputModule inputModule( &messageBus );
    messageBus.AddReceiver( &inputModule );

    ConsoleModule consoleModule( &messageBus );
    messageBus.AddReceiver( &consoleModule );
    
    GameSystemsModule gameSystemsModule(&messageBus);
    gameSystemsModule.AddEntity(&e1);
    gameSystemsModule.AddEntity(&e2);
    gameSystemsModule.AddEntity(&e3);
    gameSystemsModule.AddSystem(&mockSystem);
    messageBus.AddReceiver( &gameSystemsModule );

    //Anonymous class module
    class : public IModule
    {
        virtual void ReceiveMessage(Message msg)
        {
            if(msg.getEvent() == Event::KEY_PRESSED_S)
            {
                exit(0);
            }
        }
    } tmpExit;
        
    messageBus.AddReceiver( &tmpExit );

    //Main loop
    while (true)
    {
        inputModule.MockConsoleInput();
        messageBus.Notify();
        gameSystemsModule.Run();

        #ifdef __linux__ 
            usleep(16);
        #elif _WIN32
            Sleep(16);
        #endif
    }    

    return 0;
}