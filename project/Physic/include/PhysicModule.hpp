#ifndef PHYSICMODULE_HPP_
#define PHYSICMODULE_HPP_

#include "PhysicEventListener.hpp"

#include <reactphysics3d/reactphysics3d.h>
#include <vector>

class Entity;

/**
 * @brief Module responsible for phisic simulation.
 * Wrapper for global rp3d objects. 
 */
class PhysicModule
{
    public:
        PhysicModule() = default;
        virtual ~PhysicModule() = default;

        /**
         * @brief Initialize module by creating PhysicsWorld.
         */
        void init();

        /**
         * @brief Destroys PhysicsWorld releasing all resources. 
         */
        void cleanup();

        /**
         * @brief Apply phisic simulation, and updates necessery components. 
         */
        void physicSimulation(std::vector<Entity>* entities);

        /**
         * @brief Get the PhysicsWorld instance
         * 
         * @return rp3d::PhysicsWorld& 
         */
        inline rp3d::PhysicsWorld& GetWorld() { return *physicsWorld; }

        /**
         * @brief Get the PhysicsCommon instance
         * 
         * @return rp3d::PhysicsCommon& 
         */
        inline rp3d::PhysicsCommon& GetCommon() { return physicsCommon; }
        
    private:
        rp3d::PhysicsCommon physicsCommon;
        rp3d::PhysicsWorld* physicsWorld;
        PhysicEventListener eventListener;
};

#endif /* !PHYSICMODULE_HPP_ */
