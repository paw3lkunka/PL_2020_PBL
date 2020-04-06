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
         * */
        GameSystemsModule();

        /**
         * @brief IModule implementation, temporally does nothing
         * 
         * @param msg - Messange to handle
         */
        virtual void receiveMessage(Message msg);

        /**
         * @brief Assign ECS Entity to this object
         * 
         * @param entity pointer to Entity
         */
        void addEntity(Entity* entity);

        /**
         * @brief Assign ECS System to this object
         * 
         * @param system pointer to System
         */
        void addSystem(System* system);
        
        /**
         * @brief Runs all systems
         */
        void run();
    protected:
    private:
        std::vector<Entity*> entities;
        std::vector<System*> systems;
};

#endif /* !GAMESYSTEMS_HPP_ */
