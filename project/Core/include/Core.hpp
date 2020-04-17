#ifndef CORE_HPP_
#define CORE_HPP_

#include <vector>

#include "InputModule.hpp"
#include "ConsoleModule.hpp"
#include "MessageBus.hpp"
#include "GameSystemsModule.hpp"
#include "ResourceModule.hpp"
#include "SceneModule.hpp"

//TODO tmp includes
#include "Message.inl"
#include "Event.hpp"
#include "Transform.inl"
#include <glm/gtx/matrix_decompose.hpp> 

#include "Entity.hpp"
#include "Component.inl"
#include "System.hpp"

#include "Entity.hpp"
#include "Component.inl"
#include "System.hpp"

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
 * @brief Main class of program, contain all modules, initialization, game loop, and finalization;
 */
class Core
{
    friend Core& GetCore();
    friend void InfoLog(std::string log);
    friend void WarningLog(const char* log);
    friend void ErrorLog(const char* log);   

    public:       
#pragma region statics
        /// @brief defines initial window width.
        static constexpr int INIT_WINDOW_WIDTH = 800;
        
        /// @brief defines initial window hight.
        static constexpr int INIT_WINDOW_HIGHT = 600;

        /// @brief frame-independent time between updates in seconds.
        static constexpr double FIXED_TIME_STEP = 1.0 / 60.0;
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
        
        /// @brief resource loader and container
        ResourceModule resourceModule;
        
        /// @brief scene graph
        SceneModule sceneModule;
        
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
//TODO Es, Cs and Ss should be stored in containers/allocators, not in single variables
#pragma region mock ECS

    Entity e1 = (0, 5), e2 = (1, 5), e3 = (3, 5);
    Transform t1, t2, t3;

    class MockComponent : public Component
    {
        public:
        int a; int b;
        MockComponent(int x, int y) { a = x; b = y;}
    } mockComponent = {2, 2}, mockComponent2 = {21, 37};

    class MockInvalidComponent : public Component
    {
        bool aaa;
    } otherComponent;

    
    struct MockSystem : public System
    {
        MockComponent* ptr;
        virtual bool assertEntity(Entity* entity)
        {
            ptr = entity->getComponentPtr<MockComponent>();
            return ptr != nullptr;
        }

        virtual void frameUpdate()
        {
            int a = ptr->a, b = ptr->b;
            std::cout << a << "+" << b << "=" << a + b << std::endl;
        }
    } mockSystem;

    struct MockPositionReportSystem : public System
    {
        int counter = 1;
        Transform* t;
        virtual bool assertEntity(Entity* entity)
        {
            t = entity->getComponentPtr<Transform>();
            return t != nullptr;
        }

        virtual void fixedUpdate()
        {
            std::cout << "Mock Reporter - obj" << counter++ <<  ": " 
                << t->localToWorldMatrix[3][0] <<  ", " 
                << t->localToWorldMatrix[3][1] <<  ", " 
                << t->localToWorldMatrix[3][2] << std::endl;
        }
    } mockReporter;

    struct MockSystemWithMsgReceiver : public System, public IMsgReceiver
    {
        Transform* t;
        bool pressed = false;
        virtual bool assertEntity(Entity* entity)
        {
            t = entity->getComponentPtr<Transform>();
            return t != nullptr;
        }

        virtual void fixedUpdate()
        {
            if(pressed)
            {
                t->localPosition += glm::vec3(0,1,0);
            }
        }

        virtual void receiveMessage(Message msg)
        {
            if(msg.getEvent()==Event::KEY_PRESSED && msg.getValue<int>()==GLFW_KEY_UP)
            {
                pressed = true;
            }
        }
    } mockReceiverSystem;
#pragma endregion


    protected:
    private:
        static Core* instance;
        GLFWwindow* window; 
};

#endif /* !CORE_HPP_ */
