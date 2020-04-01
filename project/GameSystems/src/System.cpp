#include "System.hpp"

#include "Entity.hpp"

void System::Process(Entity* entity)
{
    if( AssertEntity(entity) )
    {
        Update();
    }
}