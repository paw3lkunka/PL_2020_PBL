#ifndef GAMESYSTEMS_HPP_
#define GAMESYSTEMS_HPP_

#include <vector>

#include "IModule.inl"
#include "System.hpp"

class MessageBus;
class Entity;

/**
 * @brief Module responsible for running ECS systems
 */
class GameSystemsModule : public IModule 
{
    public:        
        /**
         * @brief Construct a new GAmeSystem Module object
         * */
        GameSystemsModule();

        /**
         * @brief IModule implementation, resend messages to Systems which implements IMsgReceiver.
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
         * if system implements IMsgReceiver also connect it to MessageBus
         * @param system pointer to System
         */
        void addSystem(System* system);
        
        /**
         * @brief Runs all systems
         */
        void run(System::UpdateType updateType);
    protected:
    private:
        std::vector<Entity*> entities;
        std::vector<System*> systems;
        std::vector<IMsgReceiver*> msgReceivers;
};

#endif /* !GAMESYSTEMS_HPP_ */
