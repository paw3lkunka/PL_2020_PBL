#include "Components.inl"

template<typename T>
T* Entity::getComponentPtr(int n) const
{
    ComponentType type = ComponentEnum<T>();
    T* result = nullptr;
    for( Component* ptr : components)
    {        
        if( ptr->type == type )
        {
            result = reinterpret_cast<T*>(ptr);
            n--;
            if(n < 0)
            {
                break;
            }
        }
    }
    return result;
}