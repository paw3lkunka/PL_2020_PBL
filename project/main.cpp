#include "Core.hpp"

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


    //objMod.assetReader.loadAudioClip("Resources/Audios/sample.wav");
    
}