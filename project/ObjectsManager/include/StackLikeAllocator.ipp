
#include "StackLikeAllocator.hpp"

template<typename T>
T* StackLikeAllocator::allocate(T obj)
{
    int size = sizeof(obj);
    int freeSpace = getFreeSpace();
    if( size < freeSpace )
    {
        T* ptr = reinterpret_cast<T*>(top);
        *ptr = obj;
        top += size;
        
        return ptr;
    }
    else
    {
        throw new StackLikeAllocatorOverflowException(size, freeSpace);
    }
    
}