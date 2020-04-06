#ifndef CORE_HPP_
#define CORE_HPP_

#include <vector>

#include "InputModule.hpp"
#include "ConsoleModule.hpp"
#include "MessageBus.hpp"
#include "GameSystemsModule.hpp"

//TODO tmp includes
#include "Message.inl"
#include "Event.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class GLFWwindow;

/**
 * @brief Class containing all necessary modules 
 */
class Core
{
    public:       
#pragma region Modules
        MessageBus messageBus = (128);
        
        InputModule inputModule = (&messageBus);
        ConsoleModule consoleModule = (&messageBus);
        GameSystemsModule gameSystemsModule = (&messageBus);
        
        // TMP exit on ESC key
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
#pragma endregion

#pragma region GLFW
        GLFWwindow* window;
#pragma endregion

#pragma region Functions
        Core(){}
        ~Core() = default;

        int init();
        int mainLoop();
        void cleanup();
#pragma endregion
    protected:
    private:
};

#endif /* !CORE_HPP_ */
