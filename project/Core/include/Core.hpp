#ifndef CORE_HPP_
#define CORE_HPP_

#pragma region Includes

// * Other libs
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
#include "EditorModule.hpp"
#include "UiModule.hpp"
#include "PhysicModule.hpp"
#include "GamePlayModule.hpp"

// * ECS
#include "Entity.hpp"
#include "ComponentsPreDeclarations.hxx"
#include "SystemsPreDeclarations.hxx"

// * Others
#include <filesystem>

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

/**
 * @brief Get the Float as string With Precision given as parameter
 * 
 * @param value of float
 * @param precision of number
 * @return float with precision as string
 */
std::string getFloatWithPrecision(float value, int precision);
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
    friend class GamePlayModule;
#pragma endregion

#pragma region setup
    public:       
        /**
         * @brief Determines, if scene file should be updated during initialization.
         */
        bool updateScene = false;

        /**
         * @brief Creat scene from new_scene instead of loading file.
         */
        bool recreateScene = false;
        
        /**
         * @brief Creat scene from test_scene instead of loading file.
         */
        bool testScene = false;

        /**
         * @brief Enable opengl debugging options
         */
        bool debugOpengl = false;

        /**
         * @brief Define, if terrain chunks should be loadaed
         */
        bool loadTerrain = true;

        /**
         * @brief Path to scene file.
         */
        std::string sceneFilePath = "Resources/Scenes/mainMenuScene.json";
#pragma endregion

#pragma region Utilities
    public:
        /**
         * @brief Returns random int.
         * 
         * @return int random number.
         */
        int randomInt();

        /**
         * @brief Returns random int from [0, max] range.
         * 
         * @param max upper bound of range.
         * @return int random number.
         */
        int randomInt(int max);

        /**
         * @brief Returns random int from [min, max] range.
         * 
         * @param min lower bound of range.
         * @param max upper bound of range.
         * @return int random number.
         */
        int randomInt(int min, int max);

        /**
         * @brief Returns random float from [0, max) range.
         * 
         * @param max upper bound of range.
         * @return float random number.
         */
        float randomFloatL(float max);

        /**
         * @brief Returns random float from (0, max] range.
         * 
         * @param max upper bound of range.
         * @return float random number.
         */
        float randomFloatR(float max);
        
        /**
         * @brief Returns random float from [min, max) range.
         * 
         * @param min lower bound of range.
         * @param max upper bound of range.
         * @return float random number.
         */
        float randomFloatL(float min, float max);
        
        /**
         * @brief Returns random float from (min, max] range.
         * 
         * @param min lower bound of range.
         * @param max upper bound of range.
         * @return float random number.
         */
        float randomFloatR(float min, float max);

        /**
         * @brief Returns random float from [0, 1) range.
         * 
         * @return float random number.
         */
        float randomFloat01L();
        
        /**
         * @brief Returns random float from (0, 1] range.
         * 
         * @return float random number.
         */
        float randomFloat01R();

        /**
         * @brief Choose randomly one of two values.
         * 
         * @tparam T type of values.
         * @param obverse first value.
         * @param reverse second value.
         * @return float first value or second value.
         */
        template<typename T>
        T coinToss(T obverse, T reverse)
        {
            return randomInt() > 0 ? obverse : reverse;
        }
#pragma

#pragma region Constants
    public:       
        /// @brief identity matrix
        static constexpr glm::mat4 IDENTITY_MAT = glm::mat4(1);

        /// @brief defines initial window width.
        static constexpr int INIT_WINDOW_WIDTH = 1440;
        
        /// @brief defines window hight.
        static constexpr int INIT_WINDOW_HEIGHT = 800;

        /// @brief frame-independent time between updates in seconds.
        static constexpr double FIXED_TIME_STEP = 1.0 / 60.0;

        /// @brief frame-independent time between updates in seconds in lower precision.
        static constexpr float FIXED_TIME_STEP_F = (float)FIXED_TIME_STEP;
#pragma endregion

#pragma region Functions
    public:
        Core() = default;
        ~Core() = default;

        inline void resetFrameLagUNSAFE() { previousFrameStart = glfwGetTime(); }

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

        /**
         * @brief Get the Current Frame Start time in seconds
         * 
         * @return double 
         */
        double getCurrentFrameStart();

        /**
         * @brief is game paused flag
         */
        const bool isGamePaused() { return gamePaused; }
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

        ///@brief shows imgui editor
        EditorModule editorModule;

        ///@brief ui graph
        UiModule uiModule;

        ///@brief responsible for physic simulation
        PhysicModule physicModule;

        ///@brief gamePlay module
        GamePlayModule gamePlayModule;

#pragma endregion

#pragma region Systems
//TODO must be a better way, than static fields
    public:
        static CameraSystem cameraSystem;
        static FreeCameraControlSystem freeCameraControlSystem;
        static FirstPersonCameraControlSystem firstPersonCameraControlSystem;
        static ThirdPersonCameraControlSystem thirdPersonCameraControlSystem;
        static AudioSourceSystem audioSourceSystem;
        static AudioListenerSystem audioListenerSystem;
        static MeshRendererSystem rendererSystem;
        static TerrainRendererSystem terrainSystem;
        static PhysicalBasedInputSystem physicalBasedInputSystem;
        static PhysicSystem physicSystem;
        static SkeletonSystem skeletonSystem;
        static LightSystem lightSystem;
        static PaddleControlSystem paddleControlSystem;
        static PaddleIkSystem paddleIkSystem;
        static UiRendererSystem uiRendererSystem;
        
        //TODO documentation
        static HideoutSystem hideoutSystem;
        static UiButtonSystem uiButtonSystem;
        static EnemySystem enemySystem;
        static HydroBodySystem hydroBodySystem;
        static SortingGroupSystem sortingGroupSystem;
        static ToggleButtonSystem toggleButtonSystem;
        static CargoStorageSystem cargoStorageSystem;
        static CargoButtonSystem cargoButtonSystem;
        static DetectionBarSystem detectionBarSystem;
        static ProgressBarSystem progressBarSystem;
        static CargoSystem cargoSystem;
        static TutorialControllerSystem tutorialControllerSystem;

#pragma endregion

        /**
         * @brief Framebuffer size callback which changes size for 
         * 
         * @param window GLFW window pointer
         * @param width resized window width provided by callback
         * @param height resized window height provided by callback
         */
        static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
        static void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, 
                            GLsizei length, const GLchar *message, const GLvoid *userParam);
        static int windowWidth;
        static int windowHeight;

    private:
        static Core* instance;
        GLFWwindow* window;
        bool gamePaused = false;

        double currentFrameStart;
        double previousFrameStart;
        void sceneUnload();
        void sceneInit();
};

#endif /* !CORE_HPP_ */
