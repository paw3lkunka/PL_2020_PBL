#ifndef _HYDROBODYSYSTEM_HPP
#define _HYDROBODYSYSTEM_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/glm.hpp>

#include "System.hpp"

class Entity;

struct HydroBody;
struct HydroAccelerator;
struct Transform;
struct Rigidbody;
struct MeshRenderer;

struct HullTriangles;
struct HydroTriangle;

//TODO documentation
class HydroBodySystem : public System
{
    public:
        HydroBodySystem() = default;
        virtual ~HydroBodySystem() = default;
        
        virtual bool assertEntity(Entity* entity);
        virtual void fixedUpdate();

    private:
        HydroBody* hydroBody;
        HydroAccelerator* hydroAccelerator;
        Transform* transform;
        Rigidbody* rigidbody;
        MeshRenderer* meshRenderer;
};

#endif // _HYDROBODYSYSTEM_HPP
