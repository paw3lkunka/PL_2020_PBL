#include "Core.hpp"
#include "ResourceModule.hpp"
#include <assimp/matrix4x4.h>
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