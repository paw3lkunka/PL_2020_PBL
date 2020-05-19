#ifndef CORE_HPP_
#define CORE_HPP_

#pragma region Includes

//TODO check, which includes are really necessary

// * System-depended
#ifdef __linux__
    #include <unistd.h>
#elif _WIN32
    #include <windows.h>
#endif

// * Other libs
#include <glm/gtx/matrix_decompose.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// * MessgageBus
#include "MessageBus.hpp"

// * Modules
#include "InputModule.hpp"
#include "ConsoleModule.hpp"
#include "GameSystemsModule.hpp"
#include "RendererModule.hpp"
#include "SceneModule.hpp"
#include "AudioModule.hpp"
#include "ObjectModule.hpp"

// * ECS
#include "Entity.hpp"
#include "ComponentsPreDeclarations.hxx"
#include "SystemsPreDeclarations.hxx"

// * Others

class Message;
enum class Event : unsigned int;
class Cubemap;

#pragma endregion

#pragma region Global
class Core;
/**
 * @brief Get reference to initialized instance of Core class.
 * 
 * @return Core& 
 */
Core& GetCore();

/**
 * @brief Send info log
 * 
 * @param log text to send
 */
void InfoLog(std::string log);
/**
 * @brief Send warning log
 * 
 * @param log text to send
 */
void WarningLog(const char* log);
/**
 * @brief Send error log
 * 
 * @param log text to send
 */
void ErrorLog(const char* log);   

/**
 * @brief euler to quaternion conversion
 *
 * @param eulerAngles euler angle
 * @return glm::quat quaternion representatoin of angle
 */
glm::quat eulerToQuaternion(glm::vec3 eulerAngles);
#pragma endregion

/**
 * @brief Main class of program, contain all modules, initialization, game loop, and finalization;
 */
class Core
{
#pragma region Friendship_declarations
    friend Core& GetCore();
    friend void InfoLog(std::string log);
    friend void WarningLog(const char* log);
    friend void ErrorLog(const char* log);
#pragma endregion

#pragma region setup
    public:       
        /**
         * @brief Determines, if scene file should be updated during initialization.
         */
        bool updateScene = false;

        /**
         * @brief Creat scene from old_scene instead of loading file.
         */
        bool recreateScene = false;

        /**
         * @brief Path to scene file.
         */
        std::string sceneFilePath = "Resources/Scenes/mainScene.json";
#pragma endregion

#pragma region Constants
    public:       
        /// @brief identity matrix
        static constexpr glm::mat4 IDENTITY_MAT = glm::mat4(1);

        /// @brief defines initial window width.
        static constexpr int INIT_WINDOW_WIDTH = 1280;
        
        /// @brief defines initial window hight.
        static constexpr int INIT_WINDOW_HEIGHT = 920;

        /// @brief frame-independent time between updates in seconds.
        static constexpr double FIXED_TIME_STEP = 1.0 / 60.0;

        /// @brief frame-independent time between updates in seconds in lower precision.
        static constexpr float FIXED_TIME_STEP_F = (float)FIXED_TIME_STEP;
#pragma endregion

#pragma region Functions
    public:
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
    public:
        /// @brief implements messages exchange petween modules
        MessageBus messageBus = (1024);
        
        /// @brief reads from input devices
        InputModule inputModule;

        /// @brief print logs on terminal
        ConsoleModule consoleModule;
        
        /// @brief game logic
        GameSystemsModule gameSystemsModule;

        /// @brief rendering
        RendererModule rendererModule;
        
        /// @brief scene graph
        SceneModule sceneModule;

        /// @brief audio logic
        AudioModule audioModule;
        
        /// @brief stores all crucial objects
        ObjectModule objectModule;

        /**
         * TODO Please, do something better here ;-;
         * @brief safely close application, on ESC press
         */
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


#pragma region Systems
//TODO must be a better way, than static fields
    public:
        //TODO documentation
        static CameraSystem cameraSystem;

        //TODO documentation
        static CameraControlSystem cameraControlSystem;

        //TODO documentation
        static AudioSourceSystem audioSourceSystem;

        //TODO documentation
        static AudioListenerSystem audioListenerSystem;

        //TODO documentation
        static MeshRendererSystem rendererSystem;

        //TODO documentation
        static CollisionSystem collisionSystem;

        //TODO documentation
        static PhysicalBasedInputSystem physicalBasedInputSystem;

        //TODO documentation
        static PhysicSystem physicSystem;

        //TODO documentation
        static SkeletonSystem skeletonSystem;

        //TODO documentation
        static PaddleControlSystem paddleControlSystem;

        //TODO documentation
        static PaddleIkSystem paddleIkSystem;

#pragma endregion

        /**
         * @brief Framebuffer size callback which changes size for 
         * 
         * @param window GLFW window pointer
         * @param width resized window width provided by callback
         * @param height resized window height provided by callback
         */
        static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
        static int windowWidth, windowHeight;

    private:
        static Core* instance;
        GLFWwindow* window; 

};

#endif /* !CORE_HPP_ */
