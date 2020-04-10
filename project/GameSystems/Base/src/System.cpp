#include "System.hpp"

#include "Entity.hpp"

void System::process(Entity* entity)
{
    if( assertEntity(entity) )
    {
        update();
    }
}