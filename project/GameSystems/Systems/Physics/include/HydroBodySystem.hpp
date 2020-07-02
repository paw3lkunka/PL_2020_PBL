#ifndef _HYDROBODYSYSTEM_HPP
#define _HYDROBODYSYSTEM_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/glm.hpp>

#include "System.hpp"
#include "IMsgReceiver.inl"

class Entity;

struct HydroBody;
struct HydroAccelerator;
struct Transform;
struct Rigidbody;
struct MeshRenderer;

struct HullTriangles;
struct HydroTriangle;
struct Message;

//TODO documentation
class HydroBodySystem : public System, public IMsgReceiver
{
    public:
        HydroBodySystem() = default;
        virtual ~HydroBodySystem() = default;
        
        virtual bool assertEntity(Entity* entity);
        virtual void receiveMessage(Message msg);
        virtual void fixedUpdate();

    private:
        HydroBody* hydroBody;
        HydroAccelerator* hydroAccelerator;
        Transform* transform;
        Rigidbody* rigidbody;
        MeshRenderer* meshRenderer;

        glm::vec3 calculateCurrentForBody(HydroBody* body);
};

#endif // _HYDROBODYSYSTEM_HPP
