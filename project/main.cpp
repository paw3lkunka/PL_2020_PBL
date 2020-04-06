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
#include "Core.hpp" 
#include "InputModule.hpp"
#include "ConsoleModule.hpp"
#include "MessageBus.hpp"
#include "Message.inl"
#include "GameSystemsModule.hpp"
#include "Entity.hpp"
#include "Component.inl"
#include "System.hpp"


int main()
{
    Core core;

    if( int errorCode = core.init() )
    {
        return errorCode;
    }

    int exitCode = core.mainLoop();

    core.cleanup();

    return exitCode;
}