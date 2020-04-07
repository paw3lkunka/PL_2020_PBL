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
 * @brief Get reference to initialized instance of Core class.
 * 
 * @return Core& 
 */
Core& GetCore();

/**
 * @brief Main class of program, contain all modules, initialization, game loop, and finalization;
 */
class Core
{
    /**
     * @brief Global Getter to core instance
     * 
     * @return Core* instance of core
     */
    friend Core& GetCore();

    public:       
#pragma region statics
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
        Core() = default;
        ~Core() = default;

        /**
         * @brief Initialize Core module, and make GetCore() working, also initialize all dependencies.
         * It is impossible to initialize instane, if other instane was already initialized, and wasn't disposed by cleanup function().
         * 
         * @return - int error code:
         * 0 - No error
         * 1 - Failed to create GLFW window
         * 2 - Failed to initialize GLAD
         * 3 - Core was already initialized
         * 4 - Core was already initialized (other instance)
         */
        int init();
        /**
         * @brief Heart of application, loops while glfwWindowShouldClose() == false
         * 
         * @return int exit code, should be returned by main()
         * 0 - returned properly
         */
        int mainLoop();
        /**
         * @brief Safely dispose all dependencies 
         */
        void cleanup();
        /**
         * @brief Breaks mainLoop() propery causing exit code 0 
         */
        void close();
        /**
         * @brief Get the Message Bus object reference
         * @return MessageBus&
         */
        MessageBus& getMessageBus();
        /**
         * @brief Get the Window object pointer
         * 
         * @return GLFWwindow&*
         */
        GLFWwindow* getWindowPtr();
#pragma endregion

#pragma region Modules
        /// @brief implements messages exchange petween modules
        MessageBus messageBus = (128);
        
        /// @brief reads from input devices
        InputModule inputModule;
        /// @brief print logs on terminal
        ConsoleModule consoleModule;
        /// @brief game logic
        GameSystemsModule gameSystemsModule;
        
        /// @brief safely close application, on ESC press
        class : public IModule
        {
            virtual void receiveMessage(Message msg)
            {
                if(msg.getEvent() == Event::KEY_PRESSED && msg.getValue<int>() == GLFW_KEY_ESCAPE)
                {
                    instance->close();
                }
            }
        } tmpExit; 
#pragma endregion

    protected:
    private:
        static Core* instance;
        GLFWwindow* window; 
};

#endif /* !CORE_HPP_ */
