#include "InputModule.hpp"
#include "ConsoleModule.hpp"
#include "MessageBus.hpp"
#include <iostream>
#include <utility>
#include <windows.h>

typedef std::pair<float,float> float2;

int main()
{
    std::cout << "Henlo!" << std::endl;

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
        Sleep(16);
    }    

    return 0;
}