#include "PhysicModule.hpp"

#include "Core.hpp"
#include "ECS.inc"

void PhysicModule::init()
{
    physicsWorld = physicsCommon.createPhysicsWorld();
}

void PhysicModule::cleanup()
{
    physicsCommon.destroyPhysicsWorld(physicsWorld);
}

void PhysicModule::physicSimulation(std::vector<Entity>* entities)
{
    physicsWorld->update(Core::FIXED_TIME_STEP_F);

    for (Entity& e : *entities)
    {
        if (auto* rb = e.getComponentPtr<Rigidbody>())
        {
            reactphysics3d::Transform reactT = rb->reactRB->getTransform();

            glm::vec3 pos = Vec3Cast(reactT.getPosition());
            glm::quat rot = QuatCast(reactT.getOrientation());

            auto* tr = e.getComponentPtr<Transform>();

            tr->getLocalPositionModifiable() = tr->getToParentMatrix() * glm::vec4(pos, 1.0f);
            tr->getLocalRotationModifiable() = rot * glm::inverse(tr->getParent()->getWorldRotation());;
        }
    }
}
