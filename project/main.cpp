//#include "Core.hpp"
#include "SceneWriter.hpp"

int main()
{
    // Core core;

    // if( int errorCode = core.init() )
    // {
    //     return errorCode;
    // }

    // int exitCode = core.mainLoop();

    // core.cleanup();

    // return exitCode;

    SceneWriter sw;
    sw.tempScene();
    sw.saveScene();
}