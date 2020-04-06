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
class Core;

/**
 * @brief Main class of program, contain all modules, initialization, game loop, and finalization;
 */
class Core
{
    public:       

#pragma region Modules
        MessageBus messageBus = (128);
        
        InputModule inputModule;
        ConsoleModule consoleModule;
        GameSystemsModule gameSystemsModule;
        
        // TMP exit on ESC key
        class : public IModule
        {
            virtual void receiveMessage(Message msg)
            {
                if(msg.getEvent() == Event::KEY_PRESSED && msg.getValue<int>() == GLFW_KEY_ESCAPE)
                {
                    glfwSetWindowShouldClose(window,true);
                }
            }
        } tmpExit; 
#pragma endregion

#pragma region statics
        /**
         * @brief Get the Message Bus object
         * @return MessageBus* 
         */
        static MessageBus* getMessageBus();
        /**
         * @brief defines initial window width 
         */
        static constexpr int INIT_WINDOW_WIDTH = 800;
        
        /**
         * @brief defines initial window hight 
         */
        static constexpr int INIT_WINDOW_HIGHT = 600;
#pragma endregion

#pragma region Functions
        Core();
        ~Core() = default;

        /**
         * @brief Initialize application with all dependencies;
         * 
         * @return - int error code:
         * 0 - No error
         * 1 - Failed to create GLFW window
         * 2 - Failed to initialize GLAD
         */
        int init();
        /**
         * @brief Heart of application, loops while glfwWindowShouldClose() == false
         * 
         * @return int exit code, should be returned by main()
         * 0 - returned properly
         */
        int mainLoop();
        void cleanup();
#pragma endregion
    protected:
    private:
        static MessageBus* messageBusPtr;
        static GLFWwindow* window;
};

#endif /* !CORE_HPP_ */
