#ifndef PHYSICMODULE_HPP_
#define PHYSICMODULE_HPP_

#include "PhysicEventListener.hpp"

#include <reactphysics3d/reactphysics3d.h>
#include <vector>

class Entity;

//TODO: documentation
class PhysicModule
{
    public:
        PhysicModule() = default;
        virtual ~PhysicModule() = default;

        //TODO: documentation
        void init();

        //TODO: documentation
        void cleanup();

        /**
         * @brief Apply phisic simulation, and updates necessery components. 
         */
        void physicSimulation(std::vector<Entity>* entities);

        
        //TODO: documentation
        inline rp3d::PhysicsWorld& GetWorld() { return *physicsWorld; }

        //TODO: documentation
        inline rp3d::PhysicsCommon& GetCommon() { return physicsCommon; }
        
    private:
        rp3d::PhysicsCommon physicsCommon;
        rp3d::PhysicsWorld* physicsWorld;
        PhysicEventListener eventListener;
};

#endif /* !PHYSICMODULE_HPP_ */
