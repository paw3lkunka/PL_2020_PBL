#include "Core.hpp"
#include <iostream>
#include <ios>
#include <cstring>

int main( int argc, char * argv[] )
{
    // std::ios_base::sync_with_stdio(false);
    // std::cin.tie(NULL);
    
    Core core;

    for (int i = 1; i < argc; i++)
    {
        if( std::strcmp(argv[i], "-u") == 0 || std::strcmp(argv[i], "--update") == 0 )
        {
            core.updateScene = true;
        }
        else if( std::strcmp(argv[i], "-r") == 0 || std::strcmp(argv[i], "--recreate") == 0 )
        {
            core.recreateScene = true;
        }
        else if( std::strcmp(argv[i], "-f") == 0 || std::strcmp(argv[i], "--file") == 0 )
        {
            core.sceneFilePath = argv[++i];
        }
        else if( std::strcmp(argv[i], "-d") == 0 || std::strcmp(argv[i], "--debug") == 0 )
        {
            core.debugOpengl = true;
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