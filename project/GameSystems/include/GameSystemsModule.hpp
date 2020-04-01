#ifndef GAMESYSTEMS_HPP_
#define GAMESYSTEMS_HPP_

#include <vector>

#include "IModule.inl"

class MessageBus;
class Entity;
class System;

/**
 * @brief Module responsible for running ECS systems * 
 */
class GameSystemsModule : public IModule 
{
    public:        
        /**
         * @brief Construct a new GAmeSystem Module object
         * 
         * @param messageBus - Pointer to message bus, used to sending events
         */
        GameSystemsModule(MessageBus* messageBus);

        /**
         * @brief IModule implementation, temporally does nothing
         * 
         * @param msg - Messange to handle
         */
        virtual void ReceiveMessage(Message msg);

        /**
         * @brief Assign ECS Entity to this object
         * 
         * @param entity pointer to Entity
         */
        void AddEntity(Entity* entity);

        /**
         * @brief Assign ECS System to this object
         * 
         * @param system pointer to System
         */
        void AddSystem(System* system);
        
        /**
         * @brief Runs all systems
         * TODO: this function should be private, and called in ReceiveMessage()
         */
        void Run();
    protected:
    private:
        MessageBus* messageBus;
        std::vector<Entity*> entities;
        std::vector<System*> systems;
};

#endif /* !GAMESYSTEMS_HPP_ */
