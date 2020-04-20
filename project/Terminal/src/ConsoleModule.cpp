#include "ConsoleModule.hpp"

#include "MessageBus.hpp"
#include "Message.inl"
#include "MouseDataStructures.inl"
#include "GamepadDataStructures.inl"
#include "FileStructures.inl"
#include "AssetStructers.inl"
#include "Mesh.hpp"
#include "AudioFile.h"

ConsoleModule::ConsoleModule()
{
}       

void ConsoleModule::receiveMessage(Message msg)
{
    switch (msg.getEvent())
    {
    case Event::UNKNOWN_EVENT:
        std::cout << "console here: Unknown event was raised." << std::endl;
        break;

    case Event::DEBUG_INFO_LOG:
        std::cout << "INFO: " << msg.getValue<char*>() << std::endl;
        break;

    case Event::DEBUG_WARNING_LOG:
        std::cout << "WARNING: " << msg.getValue<char*>() << std::endl;
        break;

    case Event::DEBUG_ERROR_LOG:
        std::cerr << "ERROR: " << msg.getValue<char*>() << std::endl;
        break;

    case Event::KEY_PRESSED:
        std::cout << "console here: KEY_PRESSED. code: " << msg.getValue<int>() << std::endl;
        break;
    
    case Event::KEY_RELEASED:
        std::cout << "console here: KEY_RELEASED. code: " << msg.getValue<int>() << std::endl;
        break;
    
    case Event::KEY_REPEAT:
        std::cout << "console here: KEY_REPEAT. code: " << msg.getValue<int>() << std::endl;
        break;
    
    case Event::MOUSE_BUTTON_PRESSED:
        std::cout << "console here: MOUSE_BUTTON_PRESSED. code: " << msg.getValue<int>() << std::endl;
        break;

    case Event::MOUSE_BUTTON_RELEASED:
        std::cout << "console here: MOUSE_BUTTON_RELEASED. code: " << msg.getValue<int>() << std::endl;
        break;
        
    case Event::MOUSE_CURSOR_MOVED:
    {
        CursorData cData = msg.getValue<CursorData>();
        std::cout << "console here: MOUSE_CURSOR_MOVED. pos: " << cData.xPos << ", " << cData.yPos << ", delta: " << cData.xDelta << ", " << cData.yDelta << std::endl;
    }
        break;
    
    case Event::MOUSE_CURSOR_STOPPED:
        std::cout << "console here: MOUSE_CURSOR_STOPPED." << std::endl;
        break;
    
    case Event::MOUSE_SCROLL_MOVED:
    {
        ScrollData sData = msg.getValue<ScrollData>();
        std::cout << "console here: MOUSE_SCROLL_MOVED. " << (sData.axis ? "Vertical" : "Horizontal") << " scroll: " << sData.offset << std::endl;
    }
        break;

    case Event::GAMEPAD_CONNECTED:
        std::cout << "console here: GAMEPAD_CONNECTED. id: " << msg.getValue<int>() << std::endl;
        break;
        
    case Event::GAMEPAD_DISCONNECTED:
        std::cout << "console here: GAMEPAD_DISCONNECTED. id: " << msg.getValue<int>() << std::endl;
        break;
    
    case Event::GAMEPAD_BUTTON_PRESSED:
    {
        auto gData = msg.getValue<GamepadButtonData>();
        std::cout << "console here: GAMEPAD_BUTTON_PRESSED. gamepad: " << gData.gamepadId << ", button: " << gData.buttonId << std::endl;
    }
        break;

    case Event::GAMEPAD_BUTTON_RELEASED:
    {
        auto gData = msg.getValue<GamepadButtonData>();
        std::cout << "console here: GAMEPAD_BUTTON_RELEASED. gamepad: " << gData.gamepadId << ", button: " << gData.buttonId << std::endl;
    }
        break;

    case Event::GAMEPAD_AXIS_CHANGED:
    {
        auto gData = msg.getValue<GamepadAxisData>();
        std::cout << "console here: GAMEPAD_AXIS_CHANGED. gamepad: " << gData.gamepadId << ", axis: " << gData.axisId << ", value: " << gData.axisState << std::endl;
    }
        break;  

    case Event::LOAD_FILE:
    {
        auto fsData = msg.getValue<FileSystemData>();
        std::cout << "console here: LOAD_FILE. path: " << fsData.path << ", type:" << (int)fsData.typeOfFile << std::endl;
    }
        break;
    
    case Event::QUERY_MESH_DATA:
        std::cout << "console here: QUERY_MESH_DATA. Path: " << msg.getValue<const char*>() << std::endl;
        break;
    
    case Event::RECEIVE_MESH_DATA:
    {
        Mesh* meshData = msg.getValue<Mesh*>();
        std::cout << "console here: RECEIVE_MESH_DATA. Data address: " << &meshData << std::endl;
    }
        break;
    
    case Event::QUERY_SHADER_DATA:
        std::cout << "console here: QUERY_SHADER_DATA. Path: " << msg.getValue<const char*>() << std::endl;
        break;
    
    case Event::RECEIVE_SHADER_DATA:
    {
        std::string shaderData = msg.getValue<const char*>();
        std::cout << "console here: RECEIVE_SHADER_DATA. First 10 chars of data: " << shaderData.substr(0, 10) << std::endl;
    }
        break;

    case Event::QUERY_TEXTURE_DATA:
        std::cout << "console here: QUERY_TEXTURE_DATA. Path: " << msg.getValue<const char*>() << std::endl;
        break;
    
    case Event::RECEIVE_TEXTURE_DATA:
    {
        TextureData* texData = msg.getValue<TextureData*>();
        std::cout << "console here: RECEIVE_TEXTURE_DATA. width: " << texData->width << ", height: " << texData->height
                << ", nrComponents: " << texData->nrComponents << std::endl;    
    }
        break;

    case Event::QUERY_AUDIO_DATA:
        std::cout << "console here: QUERY_AUDIO_DATA. Path: " << msg.getValue<const char*>() << std::endl;
        break;
    
    case Event::RECEIVE_AUDIO_DATA:
    {
        AudioFile<unsigned char>* aData = msg.getValue<AudioFile<unsigned char>*>();
        std::cout << "console here: RECEIVE_AUDIO_DATA. Sample rate: " << aData->getSampleRate() << " Channels: " << aData->getNumChannels() 
                << " Bit depth: " << aData->getBitDepth() << " Length: " << aData->getLengthInSeconds() << std::endl;
    }
        break;

    case Event::RENDERER_ADD_TO_QUEUE:
        std::cout << "console here: RENDERER_ADD_TO_QUEUE.\n";
        break;

    case Event::RENDERER_SET_PROJECTION_MATRIX:
        std::cout << "console here: RENDERER_SET_PROJECTION_MATRIX.\n";
        break;

    case Event::RENDERER_SET_VIEW_MATRIX:
        std::cout << "console here: RENDERER_SET_VIEW_MATRIX.\n";
        break;

    default:
        std::cout << "console here: Event with int value: " << (int)msg.getEvent() << " was thrown." << std::endl;
        std::cout << "WARNING: Specific console response not implemented:" << std::endl;
        break;
    }
}