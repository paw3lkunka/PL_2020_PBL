#include "Components.inc"

template<typename T>
T* Entity::getComponentPtr(int n) const
{
    T* result = nullptr;
    for( Component* ptr : components)
    {        
        if( result = dynamic_cast<T*>(ptr) )
        {
            n--;
            if(n < 0)
            {
                break;
            }
        }
    }
    return result;
}

template<class T>
T* Entity::detachComponent(T* ptr)
{
    for (auto iter = components.begin(); iter < components.end(); iter++)
    {
        if (*iter == ptr)
        {
            ptr->entityPtr;
            components.erase(iter);
            return ptr;
        }
    }
    return nullptr;
}
    
template<class T>
T* Entity::detachComponent(int n)
{
    T* ptr = getComponentPtr<T>(n);
    return detachComponent(ptr);
}