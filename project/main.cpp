#include "Core.hpp"
#include <iostream>
#include <cstring>

int main( int argc, char * argv[] )
{
    Core core;

    for (int i = 1; i < argc; i++)
    {
        if( std::strcmp(argv[i], "-u") == 0 )
        {
            core.updateScene = true;
        }
        else
        {
            std::cerr << "ERROR - unknown parameter: " << argv[i] << std::endl;
            return -1;
        }
    }

    if( int errorCode = core.init() )
    {
        return errorCode;
    }

    int exitCode = core.mainLoop();

    core.cleanup();

    return exitCode;
    
}