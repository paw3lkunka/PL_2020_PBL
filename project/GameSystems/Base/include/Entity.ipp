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