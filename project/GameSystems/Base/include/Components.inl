#ifndef COMPONENTS_INC_
#define COMPONENTS_INC_

#include "Component.inl"
#include "ComponentType.inl"

#include "BoxCollider.inl"    
#include "Camera.inl"    
#include "Collider.inl"    
#include "MeshRenderer.inl"    
#include "Rigidbody.inl"    
#include "SphereCollider.inl"    
#include "Transform.inl"    

#include <typeindex>

template<typename T>
ComponentType ComponentEnum()
{
    std::type_index type(typeid(T));

    if(type == typeid(Component))
    {
        return ComponentType::COMPONENT;
    }
    else if(type == typeid(BoxCollider))
    {
        return ComponentType::BOX_COLLIDER;
    }
    else if(type == typeid(Camera))
    {
        return ComponentType::CAMERA;
    }
    else if(type == typeid(Collider))
    {
        return ComponentType::COLLIDER;
    }
    else if(type == typeid(MeshRenderer))
    {
        return ComponentType::MESH_RENDERER;
    }
    else if(type == typeid(Rigidbody))
    {
        return ComponentType::RIGIDBODY;
    }
    else if(type == typeid(SphereCollider))
    {
        return ComponentType::SPHERE_COLLIDER;
    }
    else if(type == typeid(Transform))
    {
        return ComponentType::TRANSFORM;
    }
    else
    {
        return ComponentType::INVALID_TYPE;
    }
}

#endif /* !COMPONENTS_INC_ */
