#ifndef _HYDROSURFACESYSTEM_HPP
#define _HYDROSURFACESYSTEM_HPP

#include "System.hpp"

class Entity;

struct HydroSurface;
struct Transform;
struct MeshRenderer;

class HydroSurfaceSystem : public System
{
    public:
        HydroSurfaceSystem() = default;
        virtual ~HydroSurfaceSystem() = default;
        
        virtual bool assertEntity(Entity* entity);
        virtual void fixedUpdate();

    protected:
    private:
        HydroSurface* hydroSurface;
        Transform* transform;
        MeshRenderer* meshRenderer;
};

#endif // _HYDROSURFACESYSTEM_HPP
